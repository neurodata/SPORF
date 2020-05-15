from pathlib import Path

import mne
import numpy as np
from mne_bids.read import read_raw_bids
from mne_bids.write import make_bids_basename
from scipy.io import loadmat

from read import read_dataset, read_label, read_trial

if __name__ == "__main__":

    bids_root = Path("/Users/ChesterHuynh/OneDrive - Johns Hopkins/research/data/efri/")

    # subject identifiers
    subject = "efri06"
    session = "efri"
    task = "war"
    acquisition = "seeg"
    run = "01"

    kind = "ieeg"
    trial_id = 2

    bids_fname = make_bids_basename(
        subject=subject,
        session=session,
        task=task,
        acquisition=acquisition,
        run=run,
        suffix=f"{kind}.vhdr",
    )

    bandpower_path = (
        bids_root
        / f"sub-{subject}"
        / "EFRI_bandpower"
        / f"power_bands{subject[-2:]}.mat"
    )
    bandpower_data = loadmat(bandpower_path)

    power_Hgamma = bandpower_data["power_Hgamma"].T
    # power_Lgamma = bandpower_data['power_Lgamma'].T
    # power_alpha = bandpower_data['power_alpha'].T
    # power_beta = bandpower_data['power_beta'].T
    # power_delta = bandpower_data['power_delta'].T
    # power_theta = bandpower_data['power_theta'].T

    rawdata, times, events_tsv = read_trial(
        bids_fname, bids_root, trial_id, notch_filter=True
    )

    # get the label of this trial
    labels, trial_ids = read_label(
        bids_fname, bids_root, trial_id=None, label_keyword="subject_card"
    )
    unsuccessful_trial_inds = np.where(np.isnan(labels))[
        0
    ]  # get unsuccessful trials based on keyword label
    labels = labels[~np.isnan(labels)]

    # read dataset as an epoch
    tmin = -0.1
    tmax = 1

    epochs = read_dataset(bids_fname, bids_root, tmin=tmin, tmax=tmax)
    epochs = epochs.drop(unsuccessful_trial_inds)
    epochs.load_data()
    epochs_data = epochs.get_data()

    ntrials, nchs, nsteps = epochs_data.shape

    print(power_Hgamma.shape)
