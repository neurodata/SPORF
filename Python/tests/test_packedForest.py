import pytest

from .helper import get_params
import pyfp


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
    forest.setParameter("CSVFileName", "../packedForest/res/iris.csv")
    forest.setParameter("numTreesInForest", 10)
    forest.setParameter("minParent", 1)
    forest.setParameter("columnWithY", 4)
    forest.setParameter("seed", -1661580697)
    forest.setParameter("forestType", "binnedBase")
    forest.setParameter("maxDepth", 5)
    forest._growForest()

    test_case = [5.1, 3.5, 1.4, 0.2]

    results = forest.predict_post(test_case)

    assert(len(results) == 3)
    assert(results[0] == 10)
    assert(results[1] == 0)
    assert(results[2] == 0)
