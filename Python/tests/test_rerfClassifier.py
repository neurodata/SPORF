# from the template test in sklearn:
# https://github.com/scikit-learn-contrib/project-template/blob/master/skltemplate/tests/test_template.py

import numpy as np
import pytest
from sklearn import datasets
from sklearn.utils.testing import assert_allclose, assert_array_equal
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
    assert clf.max_depth == None
    assert clf.min_parent == 1
    assert clf.max_features == "auto"
    assert clf.feature_combinations == 1.5
    assert clf.n_jobs == None
    assert clf.random_state == None

    clf.fit(iris.data, iris.target)
    assert hasattr(clf, "classes_")
    assert hasattr(clf, "X_")
    assert hasattr(clf, "y_")

    y_pred = clf.predict(iris.data)
    assert len(y_pred) == iris.target.shape[0]

    pred_proba = clf.predict_proba(iris.data)
    pred_log_proba = clf.predict_log_proba(iris.data)
    assert np.allclose(pred_proba, np.exp(pred_log_proba))


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
