import json
import logging
import os
import platform
from pathlib import Path
from typing import Union

import mne
from mne.utils import run_subprocess
from mne_bids.tsv_handler import _from_tsv
from mne_bids.tsv_handler import _to_tsv
from mne_bids.utils import _find_matching_sidecar
from mne_bids.utils import _parse_ext

logger = logging.getLogger(__name__)


def _bids_validate(bids_root):
    """Run BIDS validator."""
    shell = False
    bids_validator_exe = ["bids-validator", "--config.error=41", "--config.error=41"]
    if platform.system() == "Windows":
        shell = True
        exe = os.getenv("VALIDATOR_EXECUTABLE", "n/a")
        if "VALIDATOR_EXECUTABLE" != "n/a":
            bids_validator_exe = ["node", exe]

    def _validate(bids_root):
        cmd = bids_validator_exe + [bids_root]
        run_subprocess(cmd, shell=shell)

    return _validate(bids_root)


def add_montage(raw, bids_fname, bids_root):
    electrodes_fpath = _find_matching_sidecar(
        bids_fname, bids_root, suffix="electrodes.tsv"
    )
    coordsystem_fpath = _find_matching_sidecar(
        bids_fname, bids_root, suffix="coordsystem.json"
    )

    # load electrodes tsv
    electrodes_tsv = _from_tsv(electrodes_fpath)

    # get the coordinate frame of the data
    with open(coordsystem_fpath, "r") as fin:
        coordsystem_json = json.load(fin)
    coord_frame = coordsystem_json["iEEGCoordinateSystem"]

    # convert coordinates to float and create list of tuples
    ch_names_raw = electrodes_tsv["name"]
    ch_pos = dict(
        zip(ch_names_raw, electrodes_tsv["x"], electrodes_tsv["y"], electrodes_tsv["z"])
    )

    # create mne.DigMontage
    montage = mne.channels.make_dig_montage(ch_pos=ch_pos, coord_frame=coord_frame)
    raw.set_montage(montage)
    return raw


def append_original_fname_to_scans(
    orig_fname: str, bids_root: Union[str, Path], bids_fname: str
):
    """
    Append the original filename to scans.tsv in BIDS data structure.

    Parameters
    ----------
    orig_fname : str
    bids_root : str | Path
    bids_fname : str

    """
    # only keep the original filename w/o the extension
    bids_fname, _ = _parse_ext(bids_fname)

    # find scans.tsv sidecar
    scans_fpath = _find_matching_sidecar(
        bids_fname, bids_root, suffix="scans.tsv", allow_fail=False
    )
    scans_tsv = _from_tsv(scans_fpath)

    # new filenames
    filenames = scans_tsv["filename"]
    ind = [i for i, fname in enumerate(filenames) if bids_fname in fname]

    if len(ind) > 1:  # pragma: no cover
        raise RuntimeError(
            "This should not happen. All scans should "
            "be uniquely identifiable from scans.tsv file. "
            "The current scans file has these filenames: "
            f"{filenames}."
        )
    if len(ind) == 0:
        raise RuntimeError(
            f"No filename, {bids_fname} found. "
            f"Scans.tsv has these files: {filenames}."
        )

    # write in original filename
    if "original_filename" not in scans_tsv.keys():
        scans_tsv["original_filename"] = ["n/a"] * len(filenames)
    if scans_tsv["original_filename"][ind[0]] == "n/a":
        scans_tsv["original_filename"][ind[0]] = orig_fname
    else:
        logger.warning(
            "Original filename has already been written here. "
            f"Skipping for {bids_fname}. It is written as "
            f"{scans_tsv['original_filename'][ind[0]]}."
        )
        return

    # write the scans out
    _to_tsv(scans_tsv, scans_fpath)
