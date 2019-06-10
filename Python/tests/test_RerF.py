import numpy as np
import pytest

from .helper import get_params
from rerf.RerF import fastPredict, fastRerF, fastPredictPost


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


def test_fastPredictPost_X_Y_input():
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

    pred_first = fastPredictPost(feat_data[0, :], forest)

    # length should be number of classes
    assert len(pred_first) == 3
    # should sum to 1
    assert sum(pred_first) == 1

    # pred class should equal the max prob in posterior
    # index of max in list
    class_pred_post = max(enumerate(pred_first), key=lambda x: x[1])[0]
    assert class_pred_post == fastPredict(feat_data[0, :], forest)

    # returns a numpy array
    pred_all = fastPredictPost(feat_data, forest)

    # shape should be (num obs, num classes)
    assert pred_all.shape == (150, 3)

    # sum of each row should be 1
    assert np.array_equal(pred_all.sum(1), np.ones(150))

    # pred class should equal the max prob in posteriors
    assert np.array_equal(pred_all.argmax(axis=1), fastPredict(feat_data, forest))



def test_S_RerF():

    data_fname = "packedForest/res/mnist.csv"  # mnist
    train_X = np.genfromtxt(data_fname, delimiter=",")
    label_col = 0

    feat_data = train_X[:, 1:]  # mnist
    labels = train_X[:, label_col]

    # Take the {3,5} subset of MNIST so the test doesn't take too long.
    y35 = (labels == 3) | (labels == 5)

    feat_data = train_X[y35, 1:]  # mnist

    # Convert labels (3,5) --> (0,1)
    labels = (labels[y35] != 3).astype(int)


    forest = fastRerF(
        X=feat_data,
        Y=labels,
        forestType="S-RerF",
        trees=10,
        minParent=1,
        maxDepth=None,
        numCores=1,
        mtry=28,
        seed=30,
        imageHeight=28,
        imageWidth=28,
        patchHeightMin=1,
        patchHeightMax=4,
        patchWidthMin=1,
        patchWidthMax=4,
    )

    params = get_params(forest)

    assert params["imageHeight"] == '28'
    assert params["imageWidth"] == '28'
    assert params["patchHeightMin"] == '1'
    assert params["patchHeightMax"] == '4'
    assert params["patchWidthMin"] == '1'
    assert params["patchWidthMax"] == '4'


    # Map output labels (0,1) --> (3,5)
    train_pred = [[3,5][i] for i in fastPredict(feat_data, forest)]

    test_fname = "packedForest/res/mnist_test.csv"
    test_data = np.genfromtxt(test_fname, delimiter=",")

    test_y35 = (test_data[:, 0] == 3) | (test_data[:, 0] == 5)

    test_X = test_data[test_y35, 1:]
    test_Y = test_data[test_y35, 0]

    # Map output labels (0,1) --> (3,5)
    test_pred = [[3,5][i] for i in fastPredict(test_X, forest)]

    test_error = np.mean((test_pred != test_Y).astype(int))

    assert(test_error < 0.05) ## JLP to change this
    
