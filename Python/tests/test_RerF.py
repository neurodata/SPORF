import numpy as np
import pytest

from .helper import get_params
from ..RerF import fastPredict, fastRerF


def test_fastRerF_CSV_input():
    datafile = "packedForest/res/iris.csv"
    label_col = 4
    forest = fastRerF(
        CSVFile=datafile,
        Ycolumn=label_col,
        forestType="binnedBaseRerF",
        trees=50,
        minParent=1,
        maxDepth=None,
        numCores=1,
        mtry=2,
        seed=30,
    )

    params = get_params(forest)

    assert params["CSV file name"] == "packedForest/res/iris.csv"
    assert params["numTreesInForest"] == "50"
    assert params["minParent"] == "1"
    assert params["columnWithY"] == "4"
    assert params["numCores"] == "1"
    assert params["mtry"] == "2"
    assert params["seed"] == "30"
    assert params["Type of Forest"] == "binnedBaseRerF"
    assert params["numClasses"] == "3"
    assert params["numObservations"] == "150"
    assert params["numFeatures"] == "4"


def test_fastRerF_X_Y_input():
    datafile = "packedForest/res/iris.csv"
    X = np.genfromtxt(datafile, delimiter=",")
    feat_data = X[:, 0:4]
    Y = X[:, 4]

    forest = fastRerF(
        X=feat_data,
        Y=Y,
        forestType="binnedBaseRerF",
        trees=50,
        minParent=1,
        maxDepth=None,
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
    assert params["Type of Forest"] == "binnedBaseRerF"
    assert params["numClasses"] == "3"
    assert params["numObservations"] == "150"
    assert params["numFeatures"] == "4"


def test_fastPredict_CSV_input():
    datafile = "packedForest/res/iris.csv"
    X = np.genfromtxt(datafile, delimiter=",")
    feat_data = X[:, 0:4]
    Y = X[:, 4]
    label_col = 4
    forest = fastRerF(
        CSVFile=datafile,
        Ycolumn=label_col,
        forestType="binnedBaseRerF",
        trees=500,
        minParent=1,
        maxDepth=None,
        numCores=1,
        mtry=2,
        seed=30,
    )

    pred_first = fastPredict(feat_data[0, :], forest)
    pred_last = fastPredict(feat_data[-1, :], forest)
    pred_all = fastPredict(feat_data, forest)

    assert len(pred_all) == 150
    assert pred_first == 0
    assert pred_last == 2
    assert np.array_equal(pred_all, Y)


def test_fastPredict_X_Y_input():
    datafile = "packedForest/res/iris.csv"
    X = np.genfromtxt(datafile, delimiter=",")
    feat_data = X[:, 0:4]
    Y = X[:, 4]

    forest = fastRerF(
        X=feat_data,
        Y=Y,
        forestType="binnedBaseRerF",
        trees=500,
        minParent=1,
        maxDepth=None,
        numCores=1,
        mtry=2,
        seed=30,
    )

    pred_first = fastPredict(feat_data[0, :], forest)
    pred_last = fastPredict(feat_data[-1, :], forest)
    pred_all = fastPredict(feat_data, forest)

    assert len(pred_all) == 150
    assert pred_first == 0
    assert pred_last == 2
    assert np.array_equal(pred_all, Y)
