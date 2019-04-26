import numpy as np
import pytest

import pyfp

from .helper import get_params


def test_set_params():
    forest = pyfp.fpForest()
    forest.setParameter("CSVFileName", "somefile")
    forest.setParameter("numTreesInForest", 300)
    forest.setParameter("minParent", 2)
    forest.setParameter("columnWithY", 3)
    forest.setParameter("numCores", 4)
    forest.setParameter("fractionOfFeaturesToTest", 0.25)
    forest.setParameter("seed", 300)
    forest.setParameter("forestType", "binnedBaseRerF")
    forest.setParameter("maxDepth", 5)

    params = get_params(forest)

    assert params["CSV file name"] == "somefile"
    assert params["numTreesInForest"] == "300"
    assert params["minParent"] == "2"
    assert params["columnWithY"] == "3"
    assert params["numCores"] == "4"
    assert float(params["fractionOfFeaturesToTest"]) == 0.25
    assert params["seed"] == "300"
    assert params["Type of Forest"] == "binnedBaseRerF"
    assert params["maxDepth"] == "5"


def test_predict_post():
    forest = pyfp.fpForest()
    forest.setParameter("CSVFileName", "packedForest/res/iris.csv")
    forest.setParameter("numTreesInForest", 10)
    forest.setParameter("minParent", 1)
    forest.setParameter("columnWithY", 4)
    forest.setParameter("seed", -1661580697)
    forest.setParameter("forestType", "binnedBase")
    forest.setParameter("maxDepth", 5)
    forest._growForest()

    test_case = [5.1, 3.5, 1.4, 0.2]

    results = forest._predict_post(test_case)

    assert len(results) == 3
    assert results[0] == 10
    assert results[1] == 0
    assert results[2] == 0


def test_predict_post_numpy():
    forest = pyfp.fpForest()
    forest.setParameter("CSVFileName", "packedForest/res/iris.csv")
    forest.setParameter("numTreesInForest", 10)
    forest.setParameter("minParent", 1)
    forest.setParameter("columnWithY", 4)
    forest.setParameter("seed", -1661580697)
    forest.setParameter("forestType", "binnedBase")
    forest.setParameter("maxDepth", 5)
    forest._growForest()

    # load in 20 obs into a numpy array
    nobs = 20
    obs = np.random.rand(nobs, 4) * 5

    # returns a list of lists
    posts = forest._predict_post_array(obs)

    # length should be number of nobs
    assert len(posts) == nobs

    # sum should be nobs * numTrees
    assert sum([sum(l) for l in posts]) == 10 * nobs

    # sum should be number of trees for each obs
    for p in posts:
        assert sum(p) == 10
