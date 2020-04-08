"""API for converting files to BIDS format."""
import logging
import os
import tempfile
from pathlib import Path
from pprint import pprint
from typing import Union

import mne
from mne_bids import make_bids_basename, write_raw_bids
from natsort import natsorted
from tqdm import tqdm

from mtsmorf.io.bids_conversion import (
    _convert_mat_to_raw,
    _convert_trial_info_war,
    _create_electrodes_tsv,
    _convert_trial_info_move,
)
from mtsmorf.io.utils import append_original_fname_to_scans

logger = logging.getLogger(__name__)


def convert_mat_to_bids(
    bids_root: Union[str, os.PathLike],
    bids_basename: str,
    source_fpath: Union[str, os.PathLike],
    overwrite=True,
    verbose=False,
) -> str:
    """Run Bids conversion pipeline given filepaths."""
    print("Converting ", source_fpath, "to ", bids_basename)

    # handle conversion to raw Array
    raw = _convert_mat_to_raw(source_fpath)
    # raw = None
    # add trial info and save it
    if task == "war":
        raw = _convert_trial_info_war(source_fpath, bids_basename, bids_root, raw)
    elif task == "move":
        raw = _convert_trial_info_move(source_fpath, bids_basename, bids_root, raw)

    # create electrodes tsv file with anatomy
    _create_electrodes_tsv(source_fpath, bids_basename, bids_root)

    # get events
    events, events_id = mne.events_from_annotations(raw)

    with tempfile.TemporaryDirectory() as tmproot:
        tmpfpath = os.path.join(tmproot, "tmp_raw.fif")
        raw.save(tmpfpath)
        raw = mne.io.read_raw_fif(tmpfpath)

        # Get acceptable range of days back and pick random one
        daysback = 0

        # write to BIDS
        bids_root = write_raw_bids(
            raw,
            bids_basename,
            bids_root=str(bids_root),
            overwrite=overwrite,
            anonymize=dict(daysback=daysback, keep_his=False),
            events_data=events,
            event_id=events_id,
            verbose=verbose,
        )

    return bids_root


def _main(
    bids_root, source_path, subject_ids, acquisition, task, session
):  # pragma: no cover
    """Run Bids Conversion script to be updated.

    Just to show example run locally.
    """
    ext = "mat"

    # set BIDS kind based on acquistion
    if acquisition in ["ecog", "seeg", "ieeg"]:
        kind = "ieeg"
    elif acquisition in ["eeg"]:
        kind = "eeg"

    # go through each subject
    for subject in subject_ids:
        # get specific files
        subj_dir = Path(source_path / subject)
        rawfiles = [
            x
            for x in subj_dir.glob(f"*.{ext}")
            if task in x.name.lower()
            if "raw" in x.name.lower()
            if not x.name.startswith(".")  # make sure not a cached hidden file
        ]

        # make subject an efri number
        subject = subject.replace("SUBJECT", "efri")
        pprint(f"In {subj_dir} found {rawfiles}")

        if rawfiles == []:
            continue

        # run BIDs conversion for each separate dataset
        for run_id, fpath in enumerate(tqdm(natsorted(rawfiles)), start=1):
            logger.info(f"Running run id: {run_id}, with filepath: {fpath}")
            bids_basename = make_bids_basename(
                subject, session, task, acquisition, run_id
            )
            bids_fname = bids_basename + f"_{kind}.vhdr"

            # if any(bids_fname in x.name for x in subj_dir.rglob("*.vhdr")):
            #     continue

            # convert mat raw data into BIDs
            convert_mat_to_bids(bids_root, bids_basename, fpath, overwrite=True)

            # append scans original filenames
            append_original_fname_to_scans(
                os.path.basename(fpath), bids_root, bids_fname
            )

        # break


if __name__ == "__main__":
    # bids root to write BIDS data to
    bids_root = Path("/Users/adam2392/Downloads/vns_epilepsy/")
    bids_root = Path("/Users/adam2392/Dropbox/efri/")
    # bids_root = Path("/home/adam2392/hdd2/epilepsy_bids/")

    # path to excel layout file - would be changed to the datasheet locally
    excel_fpath = Path("/Users/patrick/Downloads/clinical_data_summary.xlsx")
    excel_fpath = Path(
        "/Users/adam2392/Dropbox/epilepsy_bids/organized_clinical_datasheet_raw.xlsx"
    )
    # excel_fpath = Path(
    #     "/home/adam2392/hdd2/epilepsy_bids/organized_clinical_datasheet_raw.xlsx"
    # )

    # define BIDS identifiers
    acquisition = "seeg"
    task = "move"
    session = "efri"

    # path to original source data
    source_path = Path(bids_root / "sourcedata")

    # HACK: get all subject ids within sourcedata
    subject_ids = natsorted(
        [
            x.name  # .replace("SUBJECT", '')
            for x in source_path.iterdir()
            if not x.as_posix().startswith(".")
            if x.is_dir()
        ]
    )
    # subject_ids = [
    #     # "pt17"
    # ]
    print(subject_ids)

    # run main bids conversion
    _main(
        bids_root, source_path, subject_ids, acquisition, task, session,
    )
