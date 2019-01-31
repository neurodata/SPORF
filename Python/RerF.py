import pyfp
import numpy as np


def fastRerF(CSVFile, Ycolumn, forestType="rerf", trees=500, minParent=1, maxDepth=0):
    """Creates a decision forest based on an input matrix and class vector.
    
    Arguments:
        CSVFile {str} -- training CSV filename
        Ycolumn {int} -- column in data with labels
    
    Keyword Arguments:
        forestType {str} -- type of forest (default: {"rerf"})
        trees {int} -- number of trees in forest (default: {500})
        minParent {int} -- minParent (default: {1})
        maxDepth {int} -- maxDepth (default: {0})
    
    Returns:
        [type] -- forest class object
    """

    forestClass = pyfp.fpForest()
    forestClass.setParameter("forestType", forestType)
    forestClass.setParameter("numTreesInForest", trees)
    forestClass.setParameter("minParent", minParent)
    forestClass.setParameter("CSVFileName", CSVFile)
    forestClass.setParameter("columnWithY", Ycolumn)

    forestClass.growForest()
    return forestClass


def fastPredict(X, forest):
    """runs a prediction on a forest with a given set of data
    
    Arguments:
        X {ndarray} -- numpy ndarray of data, if more than 1 row, run multiple predictions
        forest {forestClass} -- forest to run predictions on
    """

    X_rows = X.shape[0]

    if X_rows == 1:
        predictions = forest.predict(X.tolist())

    else:
        # predictions = map(forest.predict, [row[:].tolist() for row in X])

        predictions = np.empty(X_rows)
        for i in range(X_rows):
            predictions[i] = forest.predict(X[i, :].tolist())

    return predictions


if __name__ == "__main__":
    pass
