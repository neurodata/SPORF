import pyfp


def fastRerF(CSVFile, Ycolumn, forestType="rerf", trees=100, minParent=1):
    """Creates a decision forest based on an input matrix and class vector.  This is the main function in the rerf package.

    Arguments:
        CSVFile {[type]} -- [description]
        Ycolumn {[type]} -- [description]

    Keyword Arguments:
        forestType {str} -- [description] (default: {"rerf"})
        trees {int} -- [description] (default: {100})
        minParent {int} -- [description] (default: {1})

    Returns:
        [type] -- [description]
    """

    forestClass = pyfp.fpForest()
    forestClass.setParamString("forestType", forestType)
    forestClass.setParamInt("numTreesInForest", trees)
    forestClass.setParamInt("minParent", minParent)
    forestClass.setParamString("CSVFileName", CSVFile)
    forestClass.setParamInt("columnWithY", Ycolumn)

    forestClass.growForest()
    return forestClass
