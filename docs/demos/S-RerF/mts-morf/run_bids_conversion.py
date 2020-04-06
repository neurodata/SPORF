"""API for converting files to BIDS format."""
import logging
import os
import sys
from pathlib import Path
from pprint import pprint
from typing import Union, List
import scipy.io

from mne_bids import make_bids_basename, read_raw_bids
from mne_bids.tsv_handler import _from_tsv, _to_tsv
from mne_bids.utils import _parse_bids_filename, _find_matching_sidecar
from natsort import natsorted
from tqdm import tqdm

from eztrack.preprocess.bids_conversion import (
    write_to_bids,
    append_seeg_layout_info,
    append_subject_metadata,
    append_original_fname_to_scans,
    _bids_validate,
)
from eztrack.base.utils.file_utils import (
    _get_pt_electrode_layout,
    _update_sidecar_tsv_byname,
)

logger = logging.getLogger(__name__)


def _convert_mat_to_raw(source_fpath):
    # load in the dataset via scipy
    data_dict = scipy.io.loadmat(source_fpath)
    rawdata = data_dict['lfpdata']
    sfreq = data_dict['Fs']
    ch_names = data_dict['infos']['name']
    ch_inds = data_dict['infos']['ind']

    # load in data from misc.mat = raw preprocessed and trimmed
    misc_dict = scipy.io.loadmat(_get_misc_fname(source_fpath))
    sfreq = misc_dict['Fs']
    ch_names = misc_dict['infos']['channels']

    # load in setup.mat via scipy
    setup_dict = scipy.io.loadmat(_get_setup_fname(source_fpath))
    ch_names = setup_dict['elec_name']
    ch_anat = setup_dict['elec_area']
    trial_times = setup_dict['trial_times']  # each trial consists of 11 times/descriptions
    trial_words = setup_dict['trial_words']

    # map trial words to description
    trial_time_descriptions = _map_trial_words_to_description(trial_words)

    # compute durations
    trial_durations = _compute_durations(trial_time_description)

    for i, inds in enumerate(indices_to_change):
        duration = durations[i]
        trial_durations[inds] = duration

    # get events over the trials
    my_annot = mne.Annotations(
        onset=trial_times,
        duration=trial_durations,
        description=trial_descriptions)
    print(my_annot)

    # get the raw Array
    info = mne.CreateInfo(chs=ch_names, sfreq=sfreq)
    raw = mne.io.RawArray(rawdata, info=info)
    raw.set_annotations(my_annot)

    return raw

def convert_mat_to_bids(
    bids_root: Union[str, os.PathLike],
    bids_basename: str,
    source_fpath: Union[str, os.PathLike],
) -> str:
    """Run Bids conversion pipeline given filepaths."""
    params = _parse_bids_filename(bids_basename, verbose=False)
    acquisition = params["acq"]
    if acquisition == "eeg":
        kind = "eeg"
    elif acquisition in ["ecog", "seeg"]:
        kind = "ieeg"
    bids_fname = bids_basename + f"_{kind}.vhdr"

    # handle conversion to raw Array
    raw = _convert_mat_to_raw(source_fpath)

    # write to BIDS
    bids_root = write_raw_bids(
        raw,
        bids_basename,
        bids_root=str(bids_root),
        overwrite=True,
        anonymize=dict(daysback=daysback, keep_his=False),
        events_data=events,
        event_id=events_id,
        verbose=verbose,
    )

    bids_root = write_to_bids(source_fpath, bids_basename, bids_root)

    # run validation on the raw data
    raw = read_raw_bids(bids_fname, bids_root)
    raw.load_data()
    raw = raw.drop_channels(raw.info["bads"])
    logger.info(f"Dropping {len(raw.info['bads'])} channels as 'bad'.")
    raw = raw.pick_types(seeg=True, eeg=True, ecog=True)
    validate_raw_metadata(raw)

    return bids_root

def _compute_durations(trial_time_description):
    durations = []
    for i, description in enumerate(trial_time_description):
        description = description.lower()
        if 'show card' in description:
            durations.append(2)
        elif 'show reward' in description:
            durations.append(1.3)
        else:
            durations.append(0)
    return durations


def _get_misc_fname(fname) -> str:
    return str(fname).replace("Raw", "Misc")

def _get_setup_fname(fname) -> str:
    return str(fname).replace("Raw", "Setup")

def get_xy_fname(fname) -> str:
    return str(fname).replace("Raw", "XY")


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
        rawfiles = [x for x in subj_dir.glob(f"*.{ext}") 
            if "raw" in x.name.lower()
            if not x.name.startswith(".")  # make sure not a cached hidden file
        ]

        # make subject an efri number
        subject = subject.replace("SUBJECT", "efri")
        _get_pt_recordings(
            subj_dir, subject, ext="mat"
        )
        pprint(f"In {subj_dir} found {rawfiles}")

        # run BIDs conversion for each separate dataset
        for run_id, fpath in enumerate(tqdm(natsorted(files)), start=1):
            logger.info(f"Running run id: {run_id}, with filepath: {fpath}")
            bids_basename = make_bids_basename(
                subject, session, task, acquisition, run_id
            )
            bids_fname = bids_basename + f"_{kind}.vhdr"

            # convert mat raw data into BIDs
            convert_mat_to_bids(bids_root, bids_basename, fpath)

            # append scans original filenames
            append_original_fname_to_scans(
                os.path.basename(fpath), bids_root, bids_fname
            )


if __name__ == "__main__":
    # bids root to write BIDS data to
    bids_root = Path("/Users/adam2392/Downloads/vns_epilepsy/")
    bids_root = Path("/Users/adam2392/Downloads/tngpipeline/")
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
    task = "war"
    session = "efri"

    centers = [
        "cleveland",
        # "clevelandnl",
        # "clevelandtvb",
        # "jhu",
        # "nih",
        # 'ummc',
        # 'umf'
    ]

    for center in centers:
        # path to original source data
        source_path = Path(bids_root / "sourcedata")

        # HACK: get all subject ids within sourcedata
        subject_ids = natsorted(
            [
                x.name #.replace("SUBJECT", '')
                for x in source_path.iterdir()
                if not x.as_posix().startswith(".")
                if x.is_dir()
            ]
        )
        # subject_ids = [
        #     # "pt17"
        #     # "la05"
        #     # "umf004"
        #     # "pt15"
        #     # "la03",
        #     "la03",
        # ]
        print(subject_ids)

        # run main bids conversion
        _main(
            bids_root, source_path, subject_ids, acquisition, task, session,
        )
        # add subject metadata
        add_subject_data_from_exceldb(bids_root, subject_ids, excel_fpath)
