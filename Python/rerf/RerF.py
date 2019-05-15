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
    minParent : int, optional
        (default: 1)
    maxDepth : int, optional
        maxDepth (default: None).  If None, set to max system supported 
        value
    numCores : int, optional
        Number of cores to use (default: 1).
    mtry : int, optional
        d, the number of features to consider when splitting a node 
        (default: None).  If None, sets to ``sqrt(numFeatures)``.
    mtryMult : double, optional
        Average number of features combined to form a new feature when
        using RerF (default: 1.5)
    fractionOfFeaturesToTest : float, optional
        Sets mtry based on a fraction of the features instead of an 
        exact number (default: None).
    seed : int, optional
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
            with open(CSVFile, "r") as f:
                first_line = f.readline()
                numFeatures = len(first_line.split(","))
        else:
            raise ValueError("Need either X or CSVFile as argument")
        mtry = int(numFeatures ** (1 / 2))
    forestClass.setParameter("mtry", mtry)

    forestClass.setParameter("mtryMult", mtryMult)

    if fractionOfFeaturesToTest is not None:
        forestClass.setParameter("fractionOfFeaturesToTest", fractionOfFeaturesToTest)

    if seed is None:
        seed = np.random.randint(1, 1000000)
    forestClass.setParameter("seed", seed)

    if CSVFile is not None and Ycolumn is not None:
        forestClass.setParameter("useRowMajor", 0)
        forestClass.setParameter("CSVFileName", CSVFile)
        forestClass.setParameter("columnWithY", Ycolumn)
        forestClass._growForest()
    elif X is not None and Y is not None:
        # explicitly say we are using rowMajor
        forestClass.setParameter("useRowMajor", 1)

        num_obs = len(Y)
        num_features = X.shape[1]
        forestClass._growForestnumpy(X, Y, num_obs, num_features)

    return forestClass


def fastPredict(X, forest):
    """Predict class for X.

    The predicted class of an input sample is the majority vote by the 
    trees in the forest where each vote is the majority class of each 
    tree's leaf node.
    
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
    >>> fastPredict([0, 1, 2, 3], forest)
    """

    X = np.asarray(X)

    if X.ndim == 1:
        predictions = forest._predict(X.tolist())
    else:
        predictions = forest._predict_numpy(X)
    return predictions


def fastPredictPost(X, forest):
    """Predict class probabilities for X.

    The predicted class probabilities of an input sample are computed as 
    the normalized votes of each tree in the forest.  
    
    Parameters
    ----------
    X : array_like
        Numpy ndarray of data, if more than 1 row, run multiple 
        predictions.
    forest : pyfp.fpForest
        Forest to run predictions on

    Returns
    -------
    posterior_probabilities : list of ints, shape = [n_classes] or array, shape = [n_samples, n_classes]
        Returns the class probabilities for a single observation (list) or 
        numpy array of class probabilities for each observation depending 
        on input parameters.

    Examples
    --------
    >>> fastPredictPost([0, 1, 2, 3], forest)
    """

    X = np.asarray(X)

    if X.ndim == 1:
        y = forest._predict_post(X.tolist())
        y_prob = [p / sum(y) for p in y]
    else:
        y = forest._predict_post_array(X)
        y_arr = np.asarray(y)
        y_prob = y_arr / y_arr.sum(1)[:, None]
    return y_prob


if __name__ == "__main__":
    pass
