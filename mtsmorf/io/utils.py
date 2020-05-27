import json
import logging
import os
import platform
import re
from pathlib import Path
from typing import Union, List
import numpy as np

import mne
from mne.utils import run_subprocess
from mne_bids.tsv_handler import _from_tsv
from mne_bids.tsv_handler import _to_tsv
from mne_bids.utils import _find_matching_sidecar
from mne_bids.utils import _parse_ext

logger = logging.getLogger(__name__)


class NumpyEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, np.ndarray):
            return obj.tolist()
        return json.JSONEncoder.default(self, obj)


def plot_roc_curve():
    pass


def _generate_linspace_roc_from_dict(fpr_arr: List, tpr_arr: List, n_points: int=100):
    """Generate mean FPR and TPR curves from many ROC CV curves."""
    if len(fpr_arr) != len(tpr_arr):
        raise RuntimeError("Number of iterations through ROC curve should "
                           "be the same (i.e. fpr_arr and tpr_arr should have "
                           "the same length).")

    # create a list to store the interpolated TPRs
    tprs = []

    # create a linearly spaced FPR for reference
    mean_fpr = np.linspace(0, 1, n_points)

    for fpr, tpr in zip(fpr_arr, tpr_arr):
        # interpolate the TPR
        interp_tpr = np.interp(mean_fpr, fpr, tpr)
        interp_tpr[0] = 0.0
        tprs.append(interp_tpr)

    # compute the mean TPR along the interpolation
    mean_tpr = np.nanmean(tprs, axis=0)
    mean_tpr[-1] = 1.0

    # compute std tpr
    std_tpr = np.nanstd(tprs, axis=0)

    return mean_fpr, mean_tpr,  std_tpr


def _plot_roc_curve(mean_tpr, mean_fpr, std_tpr=None,
                    mean_auc=None, std_auc=None, ax=None):
    import matplotlib.pyplot as plt

    if ax is None:
        fig, ax = plt.subplots(1, 1)

    # plot the actual curve
    ax.plot(mean_fpr, mean_tpr, color='b',
            label=r'Mean ROC (AUC = %0.2f $\pm$ %0.2f)' % (mean_auc, std_auc),
            lw=2, alpha=.8)

    # get upper and lower bound for tpr
    tprs_upper = np.minimum(mean_tpr + std_tpr, 1)
    tprs_lower = np.maximum(mean_tpr - std_tpr, 0)
    ax.fill_between(mean_fpr, tprs_lower, tprs_upper,
                    color='grey', alpha=.2,
                    label=r'$\pm$ 1 std. dev.')

    # increase axis limits to see edges
    ax.set(xlim=[-0.05, 1.05], ylim=[-0.05, 1.05],
           title="Receiver operating characteristic curve")
    ax.legend(loc="lower right")
    return ax


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


def _get_electrode_chs(elec_name, ch_names):
    """Get a list of channels for a specific electrode."""
    elec_chs = []
    for ch in ch_names:
        elec, num = re.match("^([A-Za-z]+[']?)([0-9]+)$", ch).groups()
        if elec == elec_name:
            elec_chs.append(ch)
    return elec_chs


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
