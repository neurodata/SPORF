import json
import os
import sys
import traceback

from collections import Counter
from itertools import cycle
from pathlib import Path

import mne
import numpy as np
import pandas as pd
import seaborn as sns
from matplotlib import pyplot as plt
from sklearn.inspection import permutation_importance
from sklearn.metrics import (
    auc,
    average_precision_score,
    plot_confusion_matrix,
    precision_recall_curve,
    roc_auc_score,
    roc_curve,
)
from sklearn.model_selection import (
    cross_validate,
    KFold,
    StratifiedKFold,
    TimeSeriesSplit,
    train_test_split,
)
from sklearn.preprocessing import LabelBinarizer
from tqdm import tqdm

from mlxtend.evaluate import bootstrap_point632_score
from mne.decoding import Scaler, Vectorizer
from mne_bids.write import make_bids_basename
from rerf.rerfClassifier import rerfClassifier

# Hack-y way to import from files in sibling "io" directory
sys.path.append(str(Path(__file__).parent.parent / "io"))
sys.path.append(str(Path(__file__).parent.parent / "war_exp"))

from read import read_dataset, read_label, read_trial, get_trial_info
from plotting import plot_signals, plot_roc_multiclass, plot_feature_importances
from utils import NumpyEncoder
from cv import nested_cv_fit, cv_fit, bootstrap_fit
from traceback import print_exc


RNG = 2

if __name__ == "__main__":

    bids_root = Path("/workspaces/research/mnt/data/efri/")

    results_path = Path(
        "/workspaces/research/seeg localization/SPORF/mtsmorf/results"
    )

    ###### Some participants in the following list do not have MOVE data
    # participants = pd.read_csv(bids_root / "participants.tsv", delimiter="\t")
    # subjects = participants["participant_id"].str[4:]  # Strip prefix "sub-"
    subjects = [
        "efri02",
        "efri06",
        "efri07",
        # "efri09",  # Too few samples
        # "efri10",  # Unequal data size vs label size
        "efri13",
        "efri14",
        "efri15",
        "efri18",
        "efri20",
        "efri26",
    ]

    for subject in tqdm(subjects):

        
        # if os.path.exists(results_path / subject):
        #     continue
        # else:
        #     os.makedirs(results_path / subject)
        
        if not os.path.exists(results_path / subject):
            os.makedirs(results_path / subject)

        # subject identifiers
        session = "efri"
        task = "move"
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

        _, times, events_tsv = read_trial(
            bids_fname, bids_root, trial_id, notch_filter=True
        )

        # fetch labels
        labels, trial_ids = read_label(
            bids_fname, bids_root, trial_id=None, label_keyword="target_direction"
        )

        # we don't want perturbed trials
        behav_tsv, events_tsv = get_trial_info(bids_fname, bids_root)
        success_trial_flag = np.array(
            list(map(int, behav_tsv["successful_trial_flag"]))
        )
        success_inds = np.where(success_trial_flag == 1)[0]
        force_mag = np.array(behav_tsv["force_magnitude"], np.float64)[success_inds]

        # filter out labels for unsuccessful trials
        unsuccessful_trial_inds = np.where((np.isnan(labels) | (force_mag > 0)))[0]
        labels = np.delete(labels, unsuccessful_trial_inds)

        picks = []

        # set time window
        tmin, tmax = (-0.5, 1.0)

        # get EEG data
        epochs = read_dataset(
            bids_fname,
            bids_root,
            tmin=tmin,
            tmax=tmax,
            picks=picks,
            resample_rate=500,
            event_key="Left Target"
        )
        epochs = epochs.drop(unsuccessful_trial_inds)
        epochs.load_data()
        epochs_data = epochs.get_data()

        # get shape of data
        ntrials, nchs, nsteps = epochs_data.shape

        # check there are equal number of trials and labels
        assert ntrials == labels.shape[0], "Unequal number of trials and labels"

        # plot raw lfps
        fig, axs = plt.subplots(
            dpi=200, nrows=int(np.ceil(nchs / 4)), ncols=4, figsize=(15, 45)
        )
        axs = axs.flatten()
        plot_signals(epochs, labels, axs=axs)

        fig.tight_layout(rect=[0, 0.03, 1, 0.95])
        fig.suptitle(f"{subject.upper()}", fontsize=24)
        plt.savefig(results_path / f"{subject}/{subject}_raw_eeg_updown.png")

        # Prep data for model fitting
        included_trials = np.isin(labels, [0, 1, 2, 3])
        X = epochs_data[included_trials].reshape(np.sum(included_trials), -1)
        y = labels[included_trials]

        metrics = [
            # 'f1_micro',
            # 'f1_macro',
            "accuracy",
            "roc_auc_ovr",
        ]

        # mcnemar test
        mtsmorf = rerfClassifier(
            projection_matrix="MT-MORF",
            max_features="auto",
            n_jobs=-1,
            random_state=RNG,
            image_height=nchs,
            image_width=nsteps,
        )

        kf = StratifiedKFold(n_splits=5, shuffle=False)
        scores = cv_fit(
            mtsmorf,
            X,
            y,
            cv=kf,
            metrics=metrics,
            n_jobs=None,
            return_train_score=True,
            return_estimator=True,
        )

        clf_name = mtsmorf.get_params()["projection_matrix"]

        ################ FEATURE IMPORTANCES ################

        print("Starting feature importances...")

        n_repeats = 5
        best_ind = np.argmax(scores['test_accuracy'])
        best_estimator = scores['estimator'][best_ind]
        best_test_inds = scores['test_inds'][best_ind]

        Xtest = X[best_test_inds]
        ytest = y[best_test_inds]

        # Run feat importance for Accuracy
        scoring_method = "accuracy"
        result = permutation_importance(
            best_estimator,
            Xtest,
            ytest,
            scoring=scoring_method,
            n_repeats=n_repeats,
            n_jobs=1,
            random_state=None,
        )

        imp_std = result.importances_std
        imp_vals = result.importances_mean
        scores[f"validate_{scoring_method}_imp_mean"].append(list(imp_vals))
        scores[f"validate_{scoring_method}_imp_std"].append(list(imp_std))

        fig, ax = plt.subplots(dpi=200, figsize=(10, 10))
        plot_feature_importances(result, nchs, nsteps, n_repeats, ax=ax)
        ax.set(title=f"Feature Importances {scoring_method}")
        fig.tight_layout()

        try:
            plt.savefig(
                results_path
                / f"{subject}/{clf_name}_feature_importances_{scoring_method}.png"
            )
            print(f"Feature importance matrix {scoring_method} saved.")

        except Exception as e:
            traceback.print_exc()

        # Run feat importance for One vs Rest AUC
        scoring_method = "roc_auc_ovr"
        result = permutation_importance(
            best_estimator,
            Xtest,
            ytest,
            scoring=scoring_method,
            n_repeats=n_repeats,
            n_jobs=1,
            random_state=None,
        )

        imp_std = result.importances_std
        imp_vals = result.importances_mean
        scores[f"validate_{scoring_method}_imp_mean"].append(list(imp_vals))
        scores[f"validate_{scoring_method}_imp_std"].append(list(imp_std))

        fig, ax = plt.subplots(dpi=200, figsize=(10, 10))
        plot_feature_importances(result, nchs, nsteps, n_repeats, ax=ax)
        ax.set(title=f"Feature Importances {scoring_method}")
        fig.tight_layout()

        try:
            plt.savefig(
                results_path
                / f"{subject}/{clf_name}_feature_importances_{scoring_method}.png"
            )
            print(f"Feature importance matrix {scoring_method} saved.")

        except Exception as e:
            traceback.print_exc()

        ################ STORING RESULTS ################

        estimators = None

        try:
            if "estimator" in scores.keys():
                estimators = scores["estimator"]
                del scores["estimator"]
            with open(results_path / f"{subject}/{clf_name}_results.json", "w") as fout:
                json.dump(scores, fout, cls=NumpyEncoder)
                print("CV results saved as json.")

        except Exception as e:
            traceback.print_exc()

        if estimators is not None:
            scores["estimator"] = estimators

        for i, (estimator, test_inds) in enumerate(
            zip(scores["estimator"], scores["test_inds"])
        ):
            Xtest, ytest = X[test_inds], y[test_inds]
            fig, ax = plt.subplots(dpi=200, figsize=(10, 6))
            n_classes = 4
            plot_roc_multiclass(
                estimator,
                Xtest,
                ytest,
                n_classes,
                title=f"{subject.upper()} One vs. Rest ROC Curves",
                ax=ax,
            )

            try:
                plt.savefig(
                    results_path
                    / f"{subject}/{clf_name}_roc_curves{str(i).zfill(2)}.png"
                )
                print(f"ROC curve {i} saved.")

            except Exception as e:
                traceback.print_exc()

        for i, test_confusion_matrix in enumerate(scores["test_confusion_matrix"]):

            test_acc = np.sum(np.diagonal(test_confusion_matrix)) / np.sum(
                test_confusion_matrix
            )
            df_cm = pd.DataFrame(
                test_confusion_matrix,
                index=["down", "right", "up", "left"],
                columns=["down", "right", "up", "left"],
            )

            fig, ax = plt.subplots(dpi=100)

            sns.heatmap(df_cm, annot=True, cmap="Blues", ax=ax)
            ax.set(
                xlabel="Predicted label",
                ylabel="True label",
                title=f"{mtsmorf.get_params()['projection_matrix']} (Accuracy = {test_acc:.3f})",
            )

            try:
                plt.savefig(
                    results_path
                    / f"{subject}/{clf_name}_confusion_matrix{str(i).zfill(2)}.png"
                )
                print(f"Confusion matrix {i} saved.")

            except Exception as e:
                traceback.print_exc()
