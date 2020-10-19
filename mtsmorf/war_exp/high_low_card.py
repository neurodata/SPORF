import json
import os
import sys
from collections import Counter
from pathlib import Path
from natsort import natsorted

import joblib
import matplotlib.style as style
import mne
import numpy as np
import pandas as pd
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
from sklearn.model_selection import train_test_split, KFold, cross_validate, RandomizedSearchCV

# Hack-y way to import from files in sibling "io" directory
sys.path.append(str(Path(__file__).parent.parent / "io"))
from read import read_dataset, read_label, read_trial
from utils import NumpyEncoder

style.available
style.use("seaborn-poster")  # sets the size of the charts
style.use("ggplot")


def prep_grid(clf, apply_grid):
    if apply_grid:
        # parameter grid
        # Number of trees in random forest
        n_estimators = [int(x) for x in np.linspace(start=200, stop=1000, num=5)]
        # Number of features to consider at every split
        max_features = ["auto", "sqrt", "log2"]

        # Maximum number of levels in tree
        max_depth = [int(x) for x in np.linspace(10, 110, num=5)]
        max_depth.append(None)

        # Minimum number of samples required to split a node
        min_samples_split = [1, 2, 5, 10]

        # For RERF
        patch_height_min = [2, 3, 4, 5, 10]
        patch_width_min = [1, 5, 10, 20, 30, 40, 50, 100, 250]
        patch_height_max = [2, 3, 4, 5, 10, 15]
        patch_width_max = [10, 20, 30, 40, 50, 100, 250]

        # number of iterations to RandomSearchCV
        n_iter = 100
    else:
        n_estimators = [200]
        max_features = ["auto"]
        max_depth = [None]
        min_samples_split = [2]

        # For RERF
        patch_height_min = [2]
        patch_width_min = [20]
        patch_height_max = [10]
        patch_width_max = [50]

        # number of iterations to RandomSearchCV
        n_iter = 1

    if isinstance(clf, rerfClassifier):
        param_grid = {
            f"n_estimators": n_estimators,
            f"max_features": max_features,
            f"max_depth": max_depth,
            f"min_samples_split": min_samples_split,
            f"patch_height_min": patch_height_min,
            f"patch_width_min": patch_width_min,
            f"patch_height_max": patch_height_max,
            f"patch_width_max": patch_width_max,
        }
    
    else:
        # Get name of the classifier
        clf_name = clf.__class__.__name__
        
        param_grid = {
            f"{clf_name}__n_estimators": n_estimators,
            f"{clf_name}__max_features": max_features,
            f"{clf_name}__max_depth": max_depth,
            f"{clf_name}__min_samples_split": min_samples_split,
            f"{clf_name}__patch_height_min": patch_height_min,
            f"{clf_name}__patch_width_min": patch_width_min,
            f"{clf_name}__patch_height_max": patch_height_max,
            f"{clf_name}__patch_width_max": patch_width_max,
        }

    return param_grid


def plot_scores(non_nested_scores, nested_scores, num_trials, score_difference):
    plt.figure()
    plt.subplot(211)
    (non_nested_scores_line,) = plt.plot(non_nested_scores, color="r")
    (nested_line,) = plt.plot(nested_scores, color="b")
    plt.ylabel("score", fontsize="14")
    plt.legend(
        [non_nested_scores_line, nested_line],
        ["Non-Nested CV", "Nested CV"],
        bbox_to_anchor=(0, 0.4, 0.5, 0),
    )
    plt.title(
        "Non-Nested and Nested Cross Validation",
        x=0.5,
        y=1.1,
        fontsize="15",
    )

    # Plot bar chart of the difference.
    plt.subplot(212)
    difference_plot = plt.bar(range(num_trials), score_difference)
    plt.xlabel("Individual Trial #")
    plt.legend(
        [difference_plot],
        ["Non-Nested CV - Nested CV Score"],
        bbox_to_anchor=(0, 1, 0.8, 0),
    )
    plt.ylabel("score difference", fontsize="14")
    plt.savefig(f"./nested_cv_scores_numtrials={num_trials}")


def nested_cv_fit(clf, X, y, num_trials=1, n_splits=10, shuffle=False,
    apply_grid=False, return_scores=False, random_state=None, n_jobs=None):
    param_grid = prep_grid(clf, apply_grid)

    # Arrays to store scores
    non_nested_scores = np.zeros(num_trials)
    nested_scores = np.zeros(num_trials)

    for i in range(num_trials):
        # Choose cross-validation techniques for the inner and outer loops,
        # independently of the dataset.
        # E.g "GroupKFold", "LeaveOneOut", "LeaveOneGroupOut", etc.
        inner_cv = KFold(n_splits=n_splits, shuffle=shuffle, random_state=random_state)
        outer_cv = KFold(n_splits=n_splits, shuffle=shuffle, random_state=random_state)

        # Non-nested parameter search and scoring
        cv_clf = RandomizedSearchCV(
            estimator=clf,
            param_distributions=param_grid,
            cv=inner_cv,
            return_train_score=True,
            n_jobs=n_jobs,
        )
        cv_clf.fit(X, y)
        non_nested_scores[i] = cv_clf.best_score_

        # Store metrics
        tpr, fpr, aucs, thresholds = dict(), dict(), dict(), dict()
        test_inds = dict()

        # Store ROC metrics
        for ii, (train, test) in enumerate(outer_cv.split(X=X, y=y)):

            # For binary classification get probability for class 1
            y_proba = cv_clf.predict_proba(X[test])[:, 1]
            y_test = y[test]

            # Compute the curve and AUC
            fpr[ii], tpr[ii], thresholds[ii] = roc_curve(y_true=y_test, y_score=y_proba)
            aucs[ii] = roc_curve(y_true=y_test, y_score=y_proba)
            test_inds[ii] = test

        # Nested CV with parameter optimization
        nested_score = cross_validate(cv_clf, X=X, y=y, cv=outer_cv)
        nested_scores[i] = cv_clf.best_score_

        # save files
        fpath = f"./trial{i}_metrics_roc.json"
        with open(fpath, mode="w") as fout:
            json.dump(
                {
                    "test_inds": test_inds,
                    "tpr": tpr,
                    "fpr": fpr,
                    "thresholds": thresholds,
                },
                fout,
                cls=NumpyEncoder,
            )

        # show the differences between nested/non-nested
    score_difference = non_nested_scores - nested_scores
    avg_difference = score_difference.mean()
    std_difference = score_difference.std()
    print(
        f"Average difference of {avg_difference:.6f} with std. dev. of {std_difference:.6f}."
    )

    # Plot scores on each trial for nested and non-nested CV
    # can comment out if you don't need
    if num_trials > 1:
        plot_scores(non_nested_scores, nested_scores, num_trials, score_difference)

    print(cv_clf.best_params_)
    # Save the best parameters as a json
    with open(f"./train_{num_trials}trials_KFold_shuffle={shuffle}.json", "w") as fout:
        json.dump(cv_clf.best_params_, fout)

    if return_scores:
        return cv_clf, outer_cv, nested_scores, non_nested_scores

    return cv_clf, outer_cv



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
    bids_root = Path("/workspaces/research/mnt/data/efri/")

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

    # picks = None
    # picks = ["B'10",
    #         "D'2",
    #         "E'6",
    #         "F'1", "F'2",
    #         "I'7", "I'8",
    #         "M'4",
    #         "P'4", "P'5", "P'8", "P'9",
    #         "R'6", "R'7",
    #         ]
    # anat = [
    #     "O'1", "O'2",
    #     "P'6", "P'7",
    #     ]
    picks = []
    anat = [
        "P1",
        "P3",
        "P5",
        "P6",
        "G1",
        "G2",
        "G3",
        "G7",
        "G8",
        "V1",
        "V2",
        "V3",
        "V4",
        "V6",
        "V7",
        "V8",
    ]

    picks.extend(anat)

    # noise = ["U'9", "U'10"]
    # picks.extend(noise)
    picks = natsorted(picks)

    rawdata, times, events_tsv = read_trial(
        bids_fname, bids_root, trial_id, notch_filter=True, picks=picks
    )

    # get the label of this trial
    labels, trial_ids = read_label(
        bids_fname, bids_root, trial_id=None, label_keyword="subject_card"
    )

    # get unsuccessful trials based on keyword label
    unsuccessful_trial_inds = np.where(np.isnan(labels))[0]
    labels = np.delete(labels, unsuccessful_trial_inds)

    # read dataset as an epoch
    tmin, tmax = -0.1, 0.4
    epochs = read_dataset(
        bids_fname, bids_root, tmin=tmin, tmax=tmax, picks=picks, event_key="show card"
    )
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

    X, y = data_prep(data, labels)

    print(f"Data shape: {data.shape}")
    print(f"X shape: {X.shape}")
    print(f"y shape: {y.shape}")

    # test_size = 0.20
    # Xtrain, Xtest, ytrain, ytest = train_test_split(
    #     X, y, test_size=test_size, random_state=42
    # )

    ncores = 2
    n_runs = 1
    n_est = 500  # number of estimators

    hmin, hmax = 1, 5
    wmin, wmax = 20, 200

    clf = rerfClassifier(
        projection_matrix="MT-MORF",
        max_features="auto",
        n_jobs=ncores,
        n_estimators=n_est,
        oob_score=False,
        random_state=42,
        image_height=nchs,
        image_width=nsteps,
        patch_height_max=hmax,
        patch_height_min=hmin,
        patch_width_max=wmax,
        patch_width_min=wmin,
    )

    # Kfold cross-validation
    kf = KFold(n_splits=3, shuffle=True)

    clf = rerfClassifier()
    cv_clf, outer_cv, nested_scores, non_nested_scores = nested_cv_fit(clf, X, y,
        num_trials=10,
        n_splits=10,
        shuffle=False,
        apply_grid=False,
        return_scores=True,
        random_state=None,
        n_jobs=-1,
    )

    # mtsmorf.fit(Xtrain, ytrain)
    # ypred = mtsmorf.predict(Xtest)
    # yscore = mtsmorf.predict_proba(Xtest)
    # mtsmorf_acc = np.mean(ypred == ytest)

    # # out_dir = Path(f"../results/power={[min(freqs), max(freqs)]}Hz-channels={picks}-test_size={test_size:.2f}-t={[tmin, tmax]}")
    # out_dir = Path(f"../results/{subject}/channels={picks}-test_size={test_size:.2f}-t={[tmin, tmax]}")
    # if not os.path.exists(out_dir):
    #     os.makedirs(out_dir)

    # # Plot figures
    # filepath = out_dir / f"mtsmorf-roc-n_est={n_est}-h={[hmin, hmax]}-w={[wmin, wmax]}.png"
    # plot_roc_curve(ytest, yscore[:, 1], filepath)

    # filepath = out_dir / f"mtsmorf-pr-n_est={n_est}-h={[hmin, hmax]}-w={[wmin, wmax]}.png"
    # plot_pr_curve(ytest, yscore[:, 1], filepath)

    # filepath = out_dir / f"mtsmorf-confusion_matrix-n_est={n_est}-h={[hmin, hmax]}-w={[wmin, wmax]}.png"
    # plot_confusion_mat(mtsmorf, Xtest, ytest, filepath)

    # sporf = rerfClassifier(
    #     projection_matrix="S-RerF",
    #     max_features="auto",
    #     n_jobs=ncores,
    #     n_estimators=n_est,
    #     oob_score=False,
    #     random_state=42,
    #     image_height=nchs,
    #     image_width=nsteps,
    #     patch_height_max=hmax,
    #     patch_height_min=hmin,
    #     patch_width_max=wmax,
    #     patch_width_min=wmin,
    # )

    # sporf.fit(Xtrain, ytrain)
    # ypred = sporf.predict(Xtest)
    # yscore = sporf.predict_proba(Xtest)
    # sporf_acc = np.mean(ypred == ytest)

    # # Plot figures
    # filepath = out_dir / f"sporf-roc-n_est={n_est}-h={[hmin, hmax]}-w={[wmin, wmax]}.png"
    # plot_roc_curve(ytest, yscore[:, 1], filepath)

    # filepath = out_dir / f"sporf-pr-n_est={n_est}-h={[hmin, hmax]}-w={[wmin, wmax]}.png"
    # plot_pr_curve(ytest, yscore[:, 1], filepath)

    # filepath = out_dir / f"sporf-confusion_matrix-n_est={n_est}-h={[hmin, hmax]}-w={[wmin, wmax]}.png"
    # plot_confusion_mat(sporf, Xtest, ytest, filepath)

    # print(f"MTS-MORF accuracy: {mtsmorf_acc:.6f}")
    # print(f"SPORF accuracy: {sporf_acc:.6f}")
    # print(f"Class distribution: {Counter(ytest)}")
