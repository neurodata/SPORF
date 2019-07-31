# from the template test in sklearn:
# https://github.com/scikit-learn-contrib/project-template/blob/master/skltemplate/tests/test_template.py

import math

import numpy as np
import pytest
from sklearn import datasets, metrics
from sklearn.utils.validation import check_random_state

from rerf.rerfClassifier import rerfClassifier

# toy sample
X = [[-2, -1], [-1, -1], [-1, -2], [1, 1], [1, 2], [2, 1]]
y = [0, 0, 0, 1, 1, 1]
T = [[-1, -1], [2, 2], [3, 2]]
true_result = [0, 1, 1]

# also load the iris dataset
# and randomly permute it
iris = datasets.load_iris()
rng = check_random_state(0)
perm = rng.permutation(iris.target.size)
iris.data = iris.data[perm]
iris.target = iris.target[perm]

# also load the boston dataset
# and randomly permute it
boston = datasets.load_boston()
perm = rng.permutation(boston.target.size)
boston.data = boston.data[perm]
boston.target = boston.target[perm]


@pytest.mark.parametrize("projection_matrix", ("RerF", "Base"))
def test_classification_toy(projection_matrix):
    """Check classification on a toy dataset."""

    clf = rerfClassifier(
        projection_matrix=projection_matrix, n_estimators=10, random_state=10
    )
    clf.fit(X, y)
    assert np.array_equal(clf.predict(T), true_result)

    clf = rerfClassifier(
        projection_matrix=projection_matrix,
        n_estimators=10,
        max_features=1,
        random_state=10,
    )
    clf.fit(X, y)
    assert np.array_equal(clf.predict(T), true_result)


def test_attributes():
    clf = rerfClassifier()
    assert clf.projection_matrix == "RerF"
    assert clf.n_estimators == 500
    assert clf.max_depth is None
    assert clf.min_samples_split == 1
    assert clf.max_features == "auto"
    assert clf.feature_combinations == 1.5
    assert clf.n_jobs is None
    assert clf.random_state is None

    clf.fit(iris.data, iris.target)
    assert hasattr(clf, "classes_")
    assert hasattr(clf, "X_")
    assert hasattr(clf, "y_")
    assert hasattr(clf, "n_jobs_")

    y_pred = clf.predict(iris.data)
    assert len(y_pred) == iris.target.shape[0]

    pred_proba = clf.predict_proba(iris.data)
    pred_log_proba = clf.predict_log_proba(iris.data)
    assert np.allclose(pred_proba, np.exp(pred_log_proba))


def test_alt_attributes():
    clf = rerfClassifier(n_jobs=3, max_features=4)

    clf.fit(iris.data, iris.target)

    assert hasattr(clf, "n_jobs_")
    assert hasattr(clf, "mtry_")

    assert clf.n_jobs_ == 3
    assert clf.mtry_ == 4


def prep_digit_data():

    digits = datasets.load_digits()
    # The data that we are interested in is made of 8x8 images of digits, let's
    # have a look at the first 4 images, stored in the `images` attribute of the
    # dataset.  If we were working from image files, we could load them using
    # matplotlib.pyplot.imread.  Note that each image must have the same size. For these
    # images, we know which digit they represent: it is given in the 'target' of
    # the dataset.

    # To apply a classifier on this data, we need to flatten the image, to
    # turn the data in a (samples, feature) matrix:
    n_samples = len(digits.images)
    data = digits.images.reshape((n_samples, -1))

    return data, digits.target, n_samples


def test_s_rerf():
    # asserts that we can at least run s-rerf on some image data
    X, y, n = prep_digit_data()
    X_train = X[: n // 2]
    y_train = y[: n // 2]

    clf = rerfClassifier(
        projection_matrix="S-RerF", image_height=8, image_width=8, n_estimators=10
    )

    clf.fit(X_train, y_train)

    x_test = X[n // 2 :]
    y_test = y[n // 2 :]

    score = clf.score(x_test, y_test)

    assert score > 0.75

    assert hasattr(clf, "image_height")
    assert hasattr(clf, "image_width")
    assert hasattr(clf, "patch_width_max")
    assert hasattr(clf, "patch_width_min")
    assert hasattr(clf, "patch_height_max")
    assert hasattr(clf, "patch_height_min")

    assert clf.image_height == 8
    assert clf.image_width == 8
    assert clf.patch_width_max_ == math.floor(math.sqrt(8))
    assert clf.patch_width_min_ == 1
    assert clf.patch_height_max_ == math.floor(math.sqrt(8))
    assert clf.patch_height_min_ == 1


def check_iris_criterion(projection_matrix):
    # Check consistency on dataset iris.

    clf = rerfClassifier(
        n_estimators=10, random_state=1, projection_matrix=projection_matrix
    )
    clf.fit(iris.data, iris.target)
    score = clf.score(iris.data, iris.target)
    assert score > 0.9

    clf = rerfClassifier(
        n_estimators=10,
        max_features=2,
        random_state=1,
        projection_matrix=projection_matrix,
    )
    clf.fit(iris.data, iris.target)
    score = clf.score(iris.data, iris.target)
    assert score > 0.5


@pytest.mark.parametrize("projection_matrix", ("RerF", "Base"))
def test_iris(projection_matrix):
    check_iris_criterion(projection_matrix)


@pytest.mark.parametrize("projection_matrix", ("RerF", "Base"))
def test_iris_perfect_train(projection_matrix):
    iris_full = datasets.load_iris()
    y_train_acc_list = []
    clf = rerfClassifier(n_estimators=100, projection_matrix=projection_matrix)

    n_forests_to_try = 100
    for i in range(n_forests_to_try):
        clf.fit(iris_full.data, iris_full.target)
        y_pred_train = clf.predict(iris_full.data)
        y_train_acc_list.append(metrics.accuracy_score(iris_full.target, y_pred_train))

    correct_list = [math.isclose(yt, 1) for yt in y_train_acc_list]
    if projection_matrix == "RerF":
        # assert that we get it 100% correct all the time for RerF
        # actual accuracy was 9998 / 10K runs (5/30/2019)
        assert all(correct_list)
    elif projection_matrix == "Base":
        # assert that we get it 100% correct most of the time for RF
        # actual accuracy was 9998 / 10K runs (5/30/2019)
        assert sum(correct_list) >= 0.99 * n_forests_to_try

        # want to make sure that if any are not giving 100% acc, it's only 1 obs that's wrong
        assert min(y_train_acc_list) >= (1 - 1 / len(iris_full.target))


@pytest.mark.parametrize("projection_matrix", ("RerF", "Base"))
def test_iris_OOB(projection_matrix):
    iris_full = datasets.load_iris()

    clf = rerfClassifier(
        n_estimators=50, projection_matrix=projection_matrix, oob_score=True
    )

    clf.fit(iris_full.data, iris_full.target)

    assert 0.9 <= clf.oob_score_ < 1
