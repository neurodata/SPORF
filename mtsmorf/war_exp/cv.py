import json
import numbers
import sys
from pathlib import Path
from pprint import pprint

import joblib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from mne.decoding import Scaler, Vectorizer
from mne_bids import make_bids_basename
from sklearn.base import clone
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import (
    accuracy_score,
    f1_score,
    roc_curve,
    brier_score_loss,
    precision_score,
    recall_score,
    roc_auc_score,
    confusion_matrix,
)
from sklearn.model_selection import (
    cross_validate,
    KFold,
    RandomizedSearchCV,
    TimeSeriesSplit,
)
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import LabelBinarizer
from sklearn.utils import resample, check_random_state
from tqdm import tqdm

from rerf.rerfClassifier import rerfClassifier

# Hack-y way to import from files in sibling "io" directory
sys.path.append(str(Path(__file__).parent.parent / "io"))

from read import read_label, read_dataset
from utils import NumpyEncoder


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


def nested_cv_fit(
    clf,
    X,
    y,
    num_trials=1,
    n_splits=10,
    shuffle=False,
    apply_grid=False,
    return_scores=False,
    random_state=None,
    n_jobs=None,
):
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

    return (cv_clf, outer_cv)


def cv_fit(
    clf,
    X,
    y,
    cv=None,
    metrics=None,
    n_jobs=None,
    return_train_score=False,
    return_estimator=False,
):
    # Create a reset copy of estimator with same parameters

    # See table of sklearn metrics with a corresponding string at
    # https://scikit-learn.org/stable/modules/model_evaluation.html#scoring-parameter
    if metrics is None:
        metrics = [
            "accuracy",  # accuracy_score
            "f1",  # f1_score
            "neg_brier_score",  # brier_score_loss
            "precision",  # precision_score
            "recall",  # recall_score
            "roc_auc",  # roc_auc_score
        ]

    # Applying cross validation with specified metrics and keeping training scores and estimators.
    scores = cross_validate(
        clf,
        X,
        y,
        scoring=metrics,
        cv=cv,
        n_jobs=n_jobs,
        return_train_score=return_train_score,
        return_estimator=return_estimator,
    )

    # Appending metrics for computing ROC curve
    scores.update(cv_roc(clf, X, y, cv))

    # Appending model parameters
    scores["model_params"] = clf.get_params()

    return scores


def cv_roc(clf, X, y, cv):

    scores = {}

    scores["train_inds"] = []
    scores["train_fpr"] = []
    scores["train_tpr"] = []
    scores["train_fnr"] = []
    scores["train_tnr"] = []
    scores["train_thresholds"] = []
    scores["train_confusion_matrix"] = []

    scores["test_inds"] = []
    scores["test_fpr"] = []
    scores["test_tpr"] = []
    scores["test_fnr"] = []
    scores["test_tnr"] = []
    scores["test_thresholds"] = []
    scores["test_confusion_matrix"] = []

    tprs = []
    aucs = []
    mean_fpr = np.linspace(0, 1, 100)

    for i, (train, test) in enumerate(cv.split(X=X, y=y)):
        clf.fit(X[train], y[train])

        y_train_prob = clf.predict_proba(X[train])[:, 1]
        y_train_pred = clf.predict(X[train])
        y_train = y[train]
        cm_train = confusion_matrix(y_train, y_train_pred)

        fpr, tpr, thresholds = roc_curve(y_train, y_train_prob, pos_label=1)
        fnr, tnr, _ = roc_curve(y_train, y_train_prob, pos_label=0)

        scores["train_inds"].append(train.tolist())
        scores["train_fpr"].append(fpr.tolist())
        scores["train_tpr"].append(tpr.tolist())
        scores["train_thresholds"].append(thresholds.tolist())
        scores["train_fnr"].append(fnr.tolist())
        scores["train_tnr"].append(tnr.tolist())
        scores["train_confusion_matrix"].append(cm_train.tolist())

        # For binary classification get probability for class 1
        y_pred_prob = clf.predict_proba(X[test])[:, 1]
        y_pred = clf.predict(X[test])
        y_test_pred = clf.predict(X[test])
        y_test = y[test]

        # Compute the curve and AUC
        fpr, tpr, thresholds = roc_curve(y_test, y_pred_prob, pos_label=1)
        fnr, tnr, _ = roc_curve(y_test, y_pred_prob, pos_label=0)
        cm_test = confusion_matrix(y_test, y_test_pred)

        # roc_auc = roc_auc_score(y_test, y_pred)
        # interp_tpr = np.interp(mean_fpr, fpr, tpr)
        # interp_tpr[0] = 0.0
        # tprs.append(interp_tpr)
        # aucs.append(roc_auc)

        scores["test_inds"].append(test.tolist())
        scores["test_fpr"].append(fpr.tolist())
        scores["test_tpr"].append(tpr.tolist())
        scores["test_thresholds"].append(thresholds.tolist())
        scores["test_fnr"].append(fnr.tolist())
        scores["test_tnr"].append(tnr.tolist())
        scores["test_confusion_matrix"].append(cm_test.tolist())

    # mean_tpr = np.mean(tprs, axis=0)
    # mean_tpr[-1] = 1.0
    # mean_auc = auc(mean_fpr, mean_tpr)
    # std_auc = np.std(aucs)

    # std_tpr = np.std(tprs, axis=0)
    # tprs_upper = np.minimum(mean_tpr + std_tpr, 1)
    # tprs_lower = np.maximum(mean_tpr - std_tpr, 0)

    return scores


def bootstrap_fit(
    clf, X, y, n_samples, n_iterations=50, random_state=None, return_estimator=False
):
    scores = dict()

    scores["train_inds"] = []
    scores["test_inds"] = []

    if return_estimator:
        scores["estimator"] = []

    scores["test_acc_score"] = []
    scores["test_f1_score"] = []
    scores["test_neg_brier_score"] = []
    scores["test_precision_score"] = []
    scores["test_recall_score"] = []
    scores["test_roc_auc_score"] = []
    scores["test_fpr"] = []
    scores["test_tpr"] = []
    scores["test_thresholds"] = []
    scores["test_fnr"] = []
    scores["test_tnr"] = []
    scores["test_confusion_matrix"] = []

    # rng = check_random_state(random_state)
    # rng.seed(1)

    for i in tqdm(range(n_iterations)):

        n = len(X)
        train = resample(np.arange(n), n_samples=n_samples)
        X_train, y_train = X[train], y[train]

        test = np.array([ind for ind in np.arange(n) if ind not in train])
        X_test, y_test = X[test], y[test]

        clf.fit(X_train, y_train)
        if return_estimator:
            scores["estimator"].append(clf)

        y_pred = clf.predict(X_test)
        y_pred_prob = clf.predict_proba(X_test)[:, 1]

        fpr, tpr, thresholds = roc_curve(y_test, y_pred_prob, pos_label=1)
        fnr, tnr, _ = roc_curve(y_test, y_pred_prob, pos_label=0)
        cm = confusion_matrix(y_test, y_pred)

        scores["train_inds"].append(train.tolist())
        scores["test_inds"].append(test.tolist())

        scores["test_acc_score"].append(accuracy_score(y_test, y_pred))
        scores["test_f1_score"].append(f1_score(y_test, y_pred))
        scores["test_neg_brier_score"].append(brier_score_loss(y_test, y_pred))
        scores["test_precision_score"].append(precision_score(y_test, y_pred))
        scores["test_recall_score"].append(recall_score(y_test, y_pred))
        scores["test_roc_auc_score"].append(roc_auc_score(y_test, y_pred))

        scores["test_fpr"].append(fpr.tolist())
        scores["test_tpr"].append(tpr.tolist())
        scores["test_thresholds"].append(thresholds.tolist())
        scores["test_fnr"].append(fnr.tolist())
        scores["test_tnr"].append(tnr.tolist())
        scores["test_confusion_matrix"].append(cm.tolist())

    return scores
