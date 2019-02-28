import pyfp
import numpy as np


def fastRerF(
    X=None,
    Y=None,
    CSVFile=None,
    Ycolumn=None,
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
        X {2D numpy array} -- (default: {None})
        Y {list, 1D numpy array} -- (default: {None})
        CSVFile {str} -- training CSV filename (default: {None})
        Ycolumn {int} -- column in data with labels (default: {None})
    
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

    forestClass.setParameter("forestType", forestType)
    forestClass.setParameter("numTreesInForest", trees)
    forestClass.setParameter("minParent", minParent)

    if numCores is not None:
        forestClass.setParameter("numCores", numCores)
    if mtry is not None:
        forestClass.setParameter("mtry", mtry)
    if fractionOfFeaturesToTest is not None:
        forestClass.setParameter("fractionOfFeaturesToTest", fractionOfFeaturesToTest)
    if seed is None:
        seed = np.random.randint(1,1000000)
    forestClass.setParameter("seed", seed)

    # forestClass.setNumberOfThreads()

    if CSVFile is not None and Ycolumn is not None:
        forestClass.setParameter("CSVFileName", CSVFile)
        forestClass.setParameter("columnWithY", Ycolumn)
        forestClass._growForest()
    elif X is not None and Y is not None:
        num_obs = len(Y)
        num_features = X.shape[1]
        forestClass._growForestnumpy(X, Y, num_obs, num_features)

    return forestClass


def fastPredict(X, forest):
    """runs a prediction on a forest with a given set of data
    
    Arguments:
        X {ndarray} -- numpy ndarray of data, if more than 1 row, run multiple predictions
        forest {forestClass} -- forest to run predictions on

    Returns:
        predictions {int, list} -- int if a single row, a list if multiple observations input
    """

    if X.ndim == 1:
        predictions = forest._predict(X.tolist())
    else:
        predictions = forest._predict_numpy(X)
    return predictions


if __name__ == "__main__":
    pass
