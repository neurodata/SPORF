import pytest

from .helper import get_params
import pyfp


def test_set_params():
    forest = pyfp.fpForest()
    forest.setParamString("CSVFileName", "somefile")
    forest.setParamInt("numTreesInForest", 300)
    forest.setParamInt("minParent", 2)
    forest.setParamInt("columnWithY", 3)
    forest.setParamInt("numCores", 4)
    forest.setParamDouble("fractionOfFeaturesToTest", 0.25)
    forest.setParamInt("seed", 300)
    forest.setParamString("forestType", "rerf")

    params = get_params(forest)

    assert params["CSV file name"] == "somefile"
    assert params["numTreesInForest"] == "300"
    assert params["minParent"] == "2"
    assert params["columnWithY"] == "3"
    assert params["numCores"] == "4"
    assert float(params["fractionOfFeaturesToTest"]) == 0.25
    assert params["seed"] == "300"
    assert params["Type of Forest"] == "rerf"
