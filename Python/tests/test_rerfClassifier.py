# from the template test in sklearn:
# https://github.com/scikit-learn-contrib/project-template/blob/master/skltemplate/tests/test_template.py

import numpy as np
import pytest
from sklearn.datasets import load_iris
from sklearn.utils.testing import assert_allclose, assert_array_equal

from rerfClassifier import rerfClassifier


@pytest.fixture
def data():
    return load_iris(return_X_y=True)


def test_template_classifier(data):
    X, y = data
    clf = rerfClassifier()
    assert clf.n_estimators == 500

    clf.fit(X, y)
    assert hasattr(clf, "classes_")
    assert hasattr(clf, "X_")
    assert hasattr(clf, "y_")

    y_pred = clf.predict(X, y)

    assert y_pred.shape == (X.shape[0],)
