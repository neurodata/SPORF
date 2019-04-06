import sys

import numpy as np

import pyfp


def fastRerF(
    X=None,
    Y=None,
    CSVFile=None,
    Ycolumn=None,
    forestType="binnedBaseRerF",
    trees=500,
    minParent=1,
    maxDepth=None,
    numCores=1,
    mtry=None,
    mtryMult=1.5,
    fractionOfFeaturesToTest=None,
    seed=None,
):
    """Creates a decision forest based on an input matrix and class vector
    and grows the forest.
    
    Parameters
    ----------
    X : 2D numpy array, optional
        Input data.  Rows are observations and columns are features.
    Y : list, 1D numpy array, optional
        Labels
    CSVFile : str, optional
        training CSV filename 
    Ycolumn : int, optional
        column in data with labels 
    forestType : str, optional
        the type of forest: binnedBase, binnedBaseRerF, 
        binnedBaseTern, rfBase, rerf (default: "binnedBaseRerF")
    trees : int, optional
        Number of trees in forest (default: 500)
    minParent int, optional
        (default: 1)
    maxDepth : int
        maxDepth (default: None).  If None, set to max system supported 
        value
    numCores : int
        Number of cores to use (default: 1).
    mtry : int
        d, the number of features to consider when splitting a node 
        (default: None).  If None, sets to ``sqrt(numFeatures)``.
    mtryMult : double
        Average number of features combined to form a new feature when
        using RerF (default: 1.5)
    fractionOfFeaturesToTest : float
        Sets mtry based on a fraction of the features instead of an 
        exact number (default: None).
    seed : int
        Random seed to use (default: None).  If None, set seed to 
        ``np.random.randint(1, 1000000)``.

    Returns
    -------
    forest : pyfp.fpForest
        forest class object

    Examples
    --------
    >>> from multiprocessing import cpu_count
    >>> from sklearn.datasets import make_classification
    >>> X, y = make_classification(n_samples=1000, n_features=4,
    ...        n_informative=2, n_redundant=0,
    ...        random_state=0, shuffle=False)
    >>> forest = fastRerF(
    ...    X=X,
    ...    Y=Y,
    ...    forestType="binnedBaseRerF",
    ...    trees=500,
    ...    numCores=cpu_count(),
    ...    )
    """

    forestClass = pyfp.fpForest()

    forestClass.setParameter("forestType", forestType)
    forestClass.setParameter("numTreesInForest", trees)
    forestClass.setParameter("minParent", minParent)

    if maxDepth is not None:
        forestClass.setParameter("maxDepth", maxDepth)
    
    if numCores is None:
        numCores = 1
    forestClass.setParameter("numCores", numCores)

    if mtry is None:
        if X is not None:
            numFeatures = X.shape[1]
        elif CSVFile is not None:
            with open(CSVFile, 'r') as f:
                first_line = f.readline()
                numFeatures = len(first_line.split(','))
        else:
            raise ValueError("Need either X or CSVFile as argument")
        mtry = int(numFeatures**(1/2))
    forestClass.setParameter("mtry", mtry)

    forestClass.setParameter("mtryMult", mtryMult)
    
    if fractionOfFeaturesToTest is not None:
        forestClass.setParameter("fractionOfFeaturesToTest", fractionOfFeaturesToTest)
    
    if seed is None:
        seed = np.random.randint(1, 1000000)
    forestClass.setParameter("seed", seed)

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
    """Runs a prediction on a forest with a given set of data.
    
    Parameters
    ----------
    X : array_like
        Numpy ndarray of data, if more than 1 row, run multiple 
        predictions.
    forest : pyfp.fpForest
        Forest to run predictions on

    Returns
    -------
    predictions : int, list of int
        Returns the class of prediction (int) or predictions (list) 
        depending on input parameters.

    Examples
    --------
    >>> fastPredict(np.array([0, 0, 0, 0]), forest)
    """

    if X.ndim == 1:
        predictions = forest._predict(X.tolist())
    else:
        predictions = forest._predict_numpy(X)
    return predictions


if __name__ == "__main__":
    pass
