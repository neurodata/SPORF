import pyfp
import numpy as np


def fastRerF(
    CSVFile,
    Ycolumn,
    forestType="rerf",
    trees=500,
    minParent=1,
    maxDepth=0,
    numCores=None,
    mtry=None,
    fractionOfFeaturesToTest=None,
    seed=None,
):
    """Creates a decision forest based on an input matrix and class vector.
    
    Arguments:
        CSVFile {str} -- training CSV filename
        Ycolumn {int} -- column in data with labels
    
    Keyword Arguments:
        forestType {str} -- type of forest (default: {"rerf"})
        trees {int} -- number of trees in forest (default: {500})
        minParent {int} -- minParent (default: {1})
        maxDepth {int} -- maxDepth (default: {0})
        numCores -- number of cores to use (default: {None})
        mtry {int} -- d, number of new features created from linear combinations of input features (default: {None})
        fractionOfFeaturesToTest {float} -- fractionOfFeaturesToTest, sets mtry based on a fraction of the features instead of an exact number (default: {None})
        seed {int} -- random seed to use (default: {None})
    
    Returns:
        [pyfp.fpForest] -- forest class object
    """

    forestClass = pyfp.fpForest()
    forestClass.setParamString("forestType", forestType)
    forestClass.setParamInt("numTreesInForest", trees)
    forestClass.setParamInt("minParent", minParent)
    forestClass.setParamString("CSVFileName", CSVFile)
    forestClass.setParamInt("columnWithY", Ycolumn)

    if numCores is not None:
        forestClass.setParamInt("numCores", numCores)
    if mtry is not None:
        forestClass.setParamInt("mtry", mtry)
    if fractionOfFeaturesToTest is not None:
        forestClass.setParamDouble("fractionOfFeaturesToTest", fractionOfFeaturesToTest)
    if seed is not None:
        forestClass.setParamInt("seed", seed)

    # forestClass.setNumberOfThreads()
    forestClass.growForest()
    return forestClass


def fastPredict(X, forest):
    """runs a prediction on a forest with a given set of data
    
    Arguments:
        X {ndarray} -- numpy ndarray of data, if more than 1 row, run multiple predictions
        forest {forestClass} -- forest to run predictions on

    Returns:
        predictions {double, list} -- a double if a single row, a list if multiple predictions
    """

    X_rows = X.shape[0]

    if X_rows == 1:
        predictions = forest.predict(X.tolist())

    else:
        predictions = list(map(forest.predict, [row[:].tolist() for row in X]))

    return predictions


if __name__ == "__main__":
    pass
