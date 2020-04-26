from collections import Counter
from pathlib import Path

import numpy as np
from mne_bids.write import make_bids_basename
from sklearn.metrics import roc_auc_score
from sklearn.model_selection import train_test_split

from read import read_dataset, read_label, read_trial
from rerf.rerfClassifier import rerfClassifier


def data_prep(epochs, labels):
    X = epochs.reshape(epochs.shape[0], -1)

    # dropping trials corresponding to 6's
    print(f"Dropped {np.sum(labels == 6)} trials with subject_card = 6")
    keep_inds = np.where(labels != 6)
    y = np.where(labels[labels != 6] > 6, 1, 0)
    X = epochs[keep_inds]
    X = X.reshape(X.shape[0], -1)

    assert X.shape[0] == y.shape[0]

    return X, y


if __name__ == "__main__":
    # bids_root = Path(
    #     "/Users/ChesterHuynh/OneDrive - Johns Hopkins/research/data/efri/"
    # )

    bids_root = Path("/workspaces/research/data/efri/")

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
    epochs = read_dataset(bids_fname, bids_root, tmin=-0.1, tmax=1)
    epochs = epochs.drop(unsuccessful_trial_inds)
    epochs.load_data()
    epochs_data = epochs.get_data()

    ntrials, nchs, nsteps = epochs_data.shape

    X, y = data_prep(epochs_data, labels)

    print(f"Epochs data shape: {epochs_data.shape}")
    print(f"X shape: {X.shape}")
    print(f"y shape: {y.shape}")

    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.40, random_state=42
    )

    ncores = 1
    n_runs = 1
    n_est = 500  # number of estimators

    clf = rerfClassifier(
        projection_matrix="MT-MORF",
        max_features="auto",
        n_jobs=ncores,
        n_estimators=n_est,
        oob_score=False,
        random_state=1,
        image_height=nchs,
        image_width=nsteps,
        patch_height_max=20,
        patch_height_min=1,
        patch_width_max=500,
        patch_width_min=20,
    )

    clf.fit(X_train, y_train)
    y_pred = clf.predict(X_test)
    acc = np.mean(y_pred == y_test)
    auc = roc_auc_score(y_test, y_pred)
    print(f"Accuracy: {acc:.6f}")
    print(f"ROC AUC: {auc:.6f}")
    print(f"Class distribution: {Counter(y_test)}")

