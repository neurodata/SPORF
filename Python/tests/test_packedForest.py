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
    forest.setParameter("forestType", "rerf")

    params = get_params(forest)

    assert params["CSV file name"] == "somefile"
    assert params["numTreesInForest"] == "300"
    assert params["minParent"] == "2"
    assert params["columnWithY"] == "3"
    assert params["numCores"] == "4"
    assert float(params["fractionOfFeaturesToTest"]) == 0.25
    assert params["seed"] == "300"
    assert params["Type of Forest"] == "rerf"
