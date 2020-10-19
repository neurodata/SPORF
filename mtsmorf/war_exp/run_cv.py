import json
from pathlib import Path
from pprint import pprint

import joblib
import numpy as np
import pandas as pd
from mne.decoding import Scaler, Vectorizer
from mne_bids import make_bids_basename
from sklearn.ensemble import RandomForestClassifier
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import LabelBinarizer

from mtsmorf.io.read import read_label, read_dataset
from mtsmorf.io.utils import NumpyEncoder

# from rerf import SRerf


def cv_fit(clf, X, y, num_trials=1, apply_grid=False, apply_groups=False,
           cv_type='KFold', shuffle=False, n_splits=10, seed=1234):
    """Cross-validation experiments.

    Allows:

     i) either a grid-search (randomized), or fixed parameters,
     ii) different types of cross-validation folding
     iii) loop over many times (num_trials defaults to 1 though)
     iv) shuffling of the dataset, if we want to remove ordering of trials
     v) control of splitting

    Applies a nested cross-validation that performs bootstrapping to estimate the actual held-out
    data error.

    Parameters
    ----------
    clf :
    X :
    y :
    num_trials :
    apply_grid :
    apply_groups :
    cv_type :
    shuffle :
    n_splits :
    seed :

    Returns
    -------

    """
    import matplotlib.pyplot as plt
    from sklearn.metrics import roc_curve
    from sklearn.model_selection import (KFold, StratifiedKFold,
                                         train_test_split,
                                         TimeSeriesSplit)
    from sklearn.model_selection import (cross_val_score, RandomizedSearchCV)

    # seed things
    np.random.seed(seed)

    if cv_type == 'KFold':
        cv_func = KFold
    elif cv_type == 'StratifiedKFold':
        cv_func = StratifiedKFold
    elif cv_type == 'TimeSeriesSplit':
        cv_func = TimeSeriesSplit
    else:
        cv_func = train_test_split

    # create groups along the trial ids
    if apply_groups:
        groups = np.hstack([[ii] * (len(y) // 10) for ii in range(10)])
        if len(groups) < len(y):
            groups = np.concatenate((groups, np.tile(groups[-1], (len(y) - len(groups)))))
    else:
        groups = None

    ''' Perform CV both nested and non-nested.'''
    # Arrays to store scores
    non_nested_scores = np.zeros(num_trials)
    nested_scores = np.zeros(num_trials)

    if apply_grid:
        # parameter grid
        # Number of trees in random forest
        n_estimators = [int(x) for x in np.linspace(start=200, stop=1000, num=5)]
        # Number of features to consider at every split
        max_features = ['auto', 'sqrt', 'log2']

        # Maximum number of levels in tree
        max_depth = [int(x) for x in np.linspace(10, 110, num=5)]
        max_depth.append(None)

        # Minimum number of samples required to split a node
        min_samples_split = [1, 2, 5, 10]

        # For RERF
        patch_height_min = [2, 3, 4, 5, 10]
        patch_width_min = [1, 5, 10, 20, 30, 40, 50, 100, 500]
        patch_height_max = [2, 3, 4, 5, 10, 15]
        patch_width_max = [10, 20, 30, 40, 50, 100, 500]

        # number of iterations to RandomSearchCV
        n_iter = 100
    else:
        n_estimators = [200]
        max_features = ['auto']
        max_depth = [None]
        min_samples_split = [2]

        # For RERF
        patch_height_min = [2]
        patch_width_min = [20]
        patch_height_max = [10]
        patch_width_max = [50]

        # number of iterations to RandomSearchCV
        n_iter = 1

    # clf_name = 'randomforestclassifier'
    clf_name = 'rerfclassifier'
    random_grid = {
        f'{clf_name}__n_estimators': n_estimators,
        f'{clf_name}__max_features': max_features,
        f'{clf_name}__max_depth': max_depth,
        f'{clf_name}__min_samples_split': min_samples_split,
        f'{clf_name}__patch_height_min': patch_height_min,
        f'{clf_name}__patch_width_min': patch_width_min,
        f'{clf_name}__patch_height_max': patch_height_max,
        f'{clf_name}__patch_width_max': patch_width_max,
    }
    pprint(random_grid)

    # loop over number of trials
    for itrial in range(num_trials):
        # Choose cross-validation techniques for the inner and outer loops,
        # independently of the dataset.
        # E.g "GroupKFold", "LeaveOneOut", "LeaveOneGroupOut", etc.
        inner_cv = cv_func(n_splits=n_splits,
                           shuffle=shuffle,
                           random_state=seed)
        outer_cv = cv_func(n_splits=n_splits,
                           shuffle=shuffle,
                           random_state=seed)

        # Non_nested parameter search and scoring
        # clf = GridSearchCV(estimator=clf, param_grid=random_grid, cv=inner_cv)
        cv_clf = RandomizedSearchCV(estimator=clf,
                                    param_distributions=random_grid,
                                    n_iter=n_iter,
                                    cv=inner_cv, verbose=1,
                                    return_train_score=True,
                                    n_jobs=-1)
        print(X.shape, y.shape)
        cv_clf.fit(X, y, groups=groups)
        non_nested_scores[itrial] = cv_clf.best_score_

        # store metrics
        tpr, fpr, aucs, thresholds = dict(), dict(), dict(), dict()
        test_inds = dict()

        # store ROC metrics
        for i, (train, test) in enumerate(outer_cv.split(X=X, y=y,
                                                         groups=groups)):
            # print(train, test)
            # for binary classification get probability for class 1
            ypredict_proba = cv_clf.predict_proba(X[test])[:, 1]
            ytest = y[test]

            # print(ytest.shape, ypredict_proba.shape)
            # print(ypredict_proba)
            # print(ytest)

            # compute the curve and AUC
            fpr[i], tpr[i], thresholds[i] = roc_curve(y_true=ytest,
                                                      # pos_label=1,
                                                      y_score=ypredict_proba)
            aucs[i] = roc_curve(y_true=ytest, y_score=ypredict_proba)
            test_inds[i] = test

        # Nested CV with parameter optimization
        nested_score = cross_val_score(cv_clf, X=X, y=y, cv=outer_cv)
        nested_scores[itrial] = nested_score.mean()

        # ypredict_proba = cross_val_predict(cv_clf, X=X, y=y, groups=groups, cv=outer_cv,
        #                                    n_jobs=-1, verbose=1, method='predict_proba')

        # save files
        fpath = f'./{itrial}trial_metrics_roc.json'
        with open(fpath, mode='w') as fout:
            json.dump({'test_inds': test_inds,
                       'tpr': tpr,
                       'fpr': fpr,
                       'thresholds': thresholds},
                      fout, cls=NumpyEncoder)

        roc_df = pd.DataFrame([tpr, fpr, thresholds],
                              columns=['tpr', 'fpr', 'thresholds'])
        print(roc_df)

    # show the differences between nested/non-nested
    score_difference = non_nested_scores - nested_scores
    print("Average difference of {:6f} with std. dev. of {:6f}."
          .format(score_difference.mean(), score_difference.std()))

    # Plot scores on each trial for nested and non-nested CV
    # can comment out if you don't need
    if num_trials > 1:
        plt.figure()
        plt.subplot(211)
        non_nested_scores_line, = plt.plot(non_nested_scores, color='r')
        nested_line, = plt.plot(nested_scores, color='b')
        plt.ylabel("score", fontsize="14")
        plt.legend([non_nested_scores_line, nested_line],
                   ["Non-Nested CV", "Nested CV"],
                   bbox_to_anchor=(0, .4, .5, 0))
        plt.title("Non-Nested and Nested Cross Validation on Iris Dataset",
                  x=.5, y=1.1, fontsize="15")

        # Plot bar chart of the difference.
        plt.subplot(212)
        difference_plot = plt.bar(range(num_trials), score_difference)
        plt.xlabel("Individual Trial #")
        plt.legend([difference_plot],
                   ["Non-Nested CV - Nested CV Score"],
                   bbox_to_anchor=(0, 1, .8, 0))
        plt.ylabel("score difference", fontsize="14")
        plt.show()

    # save results
    # ReRF probably can't get pickled I believe.
    try:
        joblib.dump(cv_clf.best_estimator_, f'./train_{num_trials}trials_{cv_type}.pkl',
                    compress=1)
    except Exception as e:
        print(e)

    print(cv_clf.best_params_)
    # at the very least save the best parameters as a json
    with open(f'./train_{num_trials}trials_{cv_type}.json', 'w') as fout:
        json.dump(cv_clf.best_params_, fout)

    return cv_clf, outer_cv


def run_exp(clf, X, y, verbose=True):
    cv_clf, outer_cv = cv_fit(clf, X, y, num_trials=1,
                    cv_type='KFold', shuffle=False,
                    n_splits=2, seed=1234)
    return cv_clf, outer_cv


def _load_data(bids_basename, bids_root, label_keyword="subject_card", event_key='show card', verbose=True):
    if label_keyword is not None:
        # get labels for all trials
        y, trial_ids = read_label(
            bids_basename, bids_root, trial_id=None, label_keyword=label_keyword
        )
        unsuccessful_trial_inds = np.where(np.isnan(y))[
            0
        ]  # get unsuccessful trials based on keyword label
        y = np.delete(y, unsuccessful_trial_inds)
    else:
        y = []
        unsuccessful_trial_inds = []

    # read dataset as an epoch
    epochs = read_dataset(bids_basename, bids_root, tmin=0, tmax=0.5,
                          event_key=event_key, verbose=verbose)  # N x C x T' flatten to # N x (C x T')
    epochs = epochs.drop(unsuccessful_trial_inds)
    epochs.load_data()

    if verbose:
        print("Selected events in events.tsv: ", epochs.selection)
        print("Number of epochs: ", len(epochs))
        print("Unique labels (e.g. bet amounts): ", np.unique(y))
        print(bids_fname)

    return epochs, y


if __name__ == "__main__":
    # bids identifiers
    bids_root = Path("/Users/adam2392/Dropbox/efri/")
    # bids_root = Path("/workspaces/research/data/efri/")
    # bids_root = Path("/Users/ChesterHuynh/OneDrive - Johns Hopkins/research/data/efri/")
    deriv_path = Path(bids_root / "derivatives")

    # subject identifiers
    subject = "efri06"
    session = "efri"
    task = "war"
    acquisition = "seeg"
    run = "01"
    kind = "ieeg"
    # trial_id = 2

    # bids filename
    bids_fname = make_bids_basename(
        subject=subject,
        session=session,
        task=task,
        acquisition=acquisition,
        run=run,
        suffix=f"{kind}.vhdr",
    )
    epochs, y = _load_data(bids_fname, bids_root)

    # make sure y labels are properly binarized
    lb = LabelBinarizer(neg_label=0, pos_label=1)
    y = lb.fit_transform(y)
    print(y)

    # get the actual data
    X = epochs.get_data()

    # obtain the X data
    n_trials, image_height, image_width = X.shape

    # only keep a specific frequency band
    # gamma_power_t = epochs.copy().filter(30, 90).apply_hilbert(
    #     envelope=True).get_data()
    # alpha_power_t = epochs.copy().filter(8, 12).apply_hilbert(
    #     envelope=True).get_data()

    # run standardization

    # make sure y labels are properly binarized
    lb = LabelBinarizer(neg_label=0, pos_label=1)
    y = lb.fit_transform(y)

    verbose = True
    if verbose:
        print(X.shape)
        print("'Image' parameters of the multivariate TS raw data: ")
        print(f"{image_height} X {image_width}")
        print("Y labels are: ", np.unique(y))

    # features, so the resulting filters used are spatio-temporal
    clf = make_pipeline(
        Scaler(epochs.info),  # apply z-normalization across all channels
        Vectorizer(),
        RandomForestClassifier()
    )
    run_exp(epochs, y)
