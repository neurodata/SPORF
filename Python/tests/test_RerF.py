import numpy as np
import pytest

from .helper import get_params
from ..RerF import fastPredict, fastRerF


def test_fastRerF_CSV_input():
    datafile = "src/packedForest/res/iris.csv"
    label_col = 4
    forest = fastRerF(
        CSVFile=datafile,
        Ycolumn=label_col,
        forestType="rerf",
        trees=50,
        minParent=1,
        maxDepth=0,
        numCores=1,
        mtry=2,
        seed=30,
    )

    params = get_params(forest)

    assert params["CSV file name"] == "src/packedForest/res/iris.csv"
    assert params["numTreesInForest"] == "50"
    assert params["minParent"] == "1"
    assert params["columnWithY"] == "4"
    assert params["numCores"] == "1"
    assert params["mtry"] == "2"
    assert params["seed"] == "30"
    assert params["Type of Forest"] == "rerf"
    assert params["numClasses"] == "3"
    assert params["numObservations"] == "150"
    assert params["numFeatures"] == "4"


def test_fastRerF_X_Y_input():
    datafile = "src/packedForest/res/iris.csv"
    X = np.genfromtxt(datafile, delimiter=",")
    feat_data = X[:, 0:4]
    Y = X[:, 4]

    forest = fastRerF(
        X=feat_data,
        Y=Y,
        forestType="rerf",
        trees=50,
        minParent=1,
        maxDepth=0,
        numCores=1,
        mtry=2,
        seed=30,
    )

    params = get_params(forest)

    assert params["numTreesInForest"] == "50"
    assert params["minParent"] == "1"
    assert params["numCores"] == "1"
    assert params["mtry"] == "2"
    assert params["seed"] == "30"
    assert params["Type of Forest"] == "rerf"
    assert params["numClasses"] == "3"
    assert params["numObservations"] == "150"
    assert params["numFeatures"] == "4"
