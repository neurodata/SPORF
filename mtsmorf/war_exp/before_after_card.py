import os
from collections import Counter
from pathlib import Path
from natsort import natsorted

import matplotlib.style as style
import numpy as np
import seaborn as sns
from matplotlib import pyplot as plt
from mne_bids.write import make_bids_basename
from rerf.rerfClassifier import rerfClassifier
from sklearn.metrics import (
    average_precision_score,
    plot_confusion_matrix,
    precision_recall_curve,
    roc_auc_score,
    roc_curve,
)
from sklearn.model_selection import train_test_split

from read import read_dataset, read_label, read_trial, cv_fit
import mne

style.available
style.use("seaborn-poster")  # sets the size of the charts
style.use("ggplot")


def data_prep(t, data, labels):
    before = data[:, :, t < 0].reshape(data.shape[0], -1)  # class 0
    after = data[:, :, t > 0].reshape(data.shape[0], -1)  # class 1

    try:
        X = np.vstack([before, after])
        y = np.concatenate([np.zeros(len(before)), np.ones(len(after))])
    except ValueError:
        print("Time windows must be equal before and after 0 s.")

    assert X.shape[0] == y.shape[0]

    return X, y


def plot_roc_curve(ytrue, yscore, filepath):
    fpr, tpr, _ = roc_curve(ytrue, yscore)

    auc = roc_auc_score(ytrue, yscore)

    fig, ax = plt.subplots(dpi=300)

    sns.lineplot(x=fpr, y=tpr, label=f"MTS-MORF (area = {auc:.3f})", ci=None, ax=ax)
    sns.lineplot(x=[0, 1], y=[0, 1], color="navy", linestyle="--", ci=None, ax=ax)

    ax.set(
        xlabel="False Positive Rate",
        ylabel="True Positive Rate",
        title="MTS-MORF ROC Curve",
    )
    ax.legend()

    fig.tight_layout()

    plt.savefig(filepath)


def plot_pr_curve(ytrue, yscore, filepath):
    precision, recall, _ = precision_recall_curve(ytrue, yscore)

    average_precision = average_precision_score(ytrue, yscore)

    fig, ax = plt.subplots(dpi=300)

    sns.lineplot(
        x=recall,
        y=precision,
        label=f"MTS-MORF (AP = {average_precision:.3f})",
        ci=None,
        ax=ax,
    )

    ax.set(xlabel="Recall", ylabel="Precision", title="MTS-MORF P-R Curve")
    ax.legend()

    fig.tight_layout()

    plt.savefig(filepath)


def plot_confusion_mat(clf, Xtest, ytest, filepath):
    fig, ax = plt.subplots(dpi=300)

    plot_confusion_matrix(clf, Xtest, ytest, ax=ax, cmap=plt.cm.Blues)

    ax.set(title="MTS-MORF Confusion Matrix")
    fig.tight_layout()
    plt.savefig(filepath)


if __name__ == "__main__":
    bids_root = Path("/workspaces/research/data/efri/")

    # subject identifiers
    subject = "efri07"
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

    # picks = set(
    #     ["B8", "B9", "B10", "C6", "C7", "C8", "P1", "P8", "U3", "V8", "W8", "W9", "X9"]
    # )
    # anat = set(
    #     [
    #         # "P1", "P3", "P5", "P6", "P8",
    #         # "G1", "G2", "G3", "G7", "G8",
    #         # "V1", "V2", "V3", "V4", "V6", "V7", "V8",
    #     ]
    # )
    # anat = []

    # picks.update(anat)
    # picks = list(picks)

    # noise = ["U'9", "U'10"]
    # picks.extend(noise)
    # picks = natsorted(picks)
    picks = None

    rawdata, times, events_tsv = read_trial(
        bids_fname, bids_root, trial_id, notch_filter=True, picks=picks
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
    tmin, tmax = -0.2, 0.2
    epochs = read_dataset(bids_fname, bids_root, tmin=tmin, tmax=tmax, picks=picks)
    epochs = epochs.drop(unsuccessful_trial_inds)
    epochs.load_data()
    epochs_data = epochs.get_data()
    data = epochs_data  # raw LFPs

    # freqs = np.linspace(70, 80, num=30)
    # band-pass filter, raw.filter(low, high)

    # power = mne.time_frequency.tfr_morlet(epochs, freqs=freqs, n_cycles=3, decim=2,
    #                                         n_jobs=2, return_itc=False, average=False)
    # power = power.apply_baseline(baseline=(None, None), mode='mean')  # (ntrials, nchannels, nfreqs, nsteps)
    # power_data = np.mean(power.data, axis=2)
    # data = power_data  # avg power for specified freq range

    ntrials, nchs, nsteps = data.shape
    t = epochs.times

    X, y = data_prep(t, data, labels)

    print(f"Data shape: {data.shape}")
    print(f"X shape: {X.shape}")
    print(f"y shape: {y.shape}")

    test_size = 0.3
    Xtrain, Xtest, ytrain, ytest = train_test_split(
        X, y, test_size=test_size, random_state=42
    )

    ncores = 2
    n_runs = 1
    n_est = 500  # number of estimators

    hmin, hmax = 1, nchs
    wmin, wmax = 1, len(t[t < 0])

    mtsmorf = rerfClassifier(
        projection_matrix="MT-MORF",
        max_features="auto",
        n_jobs=ncores,
        n_estimators=n_est,
        oob_score=False,
        random_state=42,
        image_height=nchs,
        image_width=len(t[t < 0]),
        patch_height_max=hmax,
        patch_height_min=hmin,
        patch_width_max=wmax,
        patch_width_min=wmin,
    )

    # mtsmorf_cv = cv_fit(
    #     mtsmorf,
    #     X,
    #     y,
    #     num_trials=1,
    #     apply_grid=True,
    #     cv_type="KFold",
    #     shuffle=True,
    #     n_splits=10,
    #     seed=1234,
    # )
    mtsmorf.fit(Xtrain, ytrain)
    ypred = mtsmorf.predict(Xtest)
    yscore = mtsmorf.predict_proba(Xtest)
    mtsmorf_acc = np.mean(ypred == ytest)

    channels = picks if picks is not None else "all"
    out_dir = Path(f"../results/before-after/{subject}/channels={channels}-test_size={test_size:.2f}-t={[tmin, tmax]}")
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    # Plot figures
    filepath = out_dir / f"mtsmorf-roc-n_est={n_est}-h={[hmin, hmax]}-w={[wmin, wmax]}.png"
    plot_roc_curve(ytest, yscore[:, 1], filepath)

    filepath = out_dir / f"mtsmorf-pr-n_est={n_est}-h={[hmin, hmax]}-w={[wmin, wmax]}.png"
    plot_pr_curve(ytest, yscore[:, 1], filepath)

    filepath = out_dir / f"mtsmorf-confusion_matrix-n_est={n_est}-h={[hmin, hmax]}-w={[wmin, wmax]}.png"
    plot_confusion_mat(mtsmorf, Xtest, ytest, filepath)

    sporf = rerfClassifier(
        projection_matrix="S-RerF",
        max_features="auto",
        n_jobs=ncores,
        n_estimators=n_est,
        oob_score=False,
        random_state=42,
        image_height=nchs,
        image_width=len(t[t < 0]),
        patch_height_max=hmax,
        patch_height_min=hmin,
        patch_width_max=wmax,
        patch_width_min=wmin,
    )

    # sporf_cv = cv_fit(
    #     sporf,
    #     X,
    #     y,
    #     num_trials=1,
    #     apply_grid=True,
    #     cv_type="KFold",
    #     shuffle=True,
    #     n_splits=10,
    #     seed=1234,
    # )

    sporf.fit(Xtrain, ytrain)
    ypred = sporf.predict(Xtest)
    yscore = sporf.predict_proba(Xtest)
    sporf_acc = np.mean(ypred == ytest)

    # Plot figures
    filepath = out_dir / f"sporf-roc-n_est={n_est}-h={[hmin, hmax]}-w={[wmin, wmax]}.png"
    plot_roc_curve(ytest, yscore[:, 1], filepath)

    filepath = out_dir / f"sporf-pr-n_est={n_est}-h={[hmin, hmax]}-w={[wmin, wmax]}.png"
    plot_pr_curve(ytest, yscore[:, 1], filepath)

    filepath = out_dir / f"sporf-confusion_matrix-n_est={n_est}-h={[hmin, hmax]}-w={[wmin, wmax]}.png"
    plot_confusion_mat(sporf, Xtest, ytest, filepath)

    print(f"MTS-MORF accuracy: {mtsmorf_acc:.6f}")
    print(f"SPORF accuracy: {sporf_acc:.6f}")
    print(f"Class distribution: {Counter(ytest)}")
