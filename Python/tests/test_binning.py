import numpy as np
import pytest

from .helper import get_params
from rerf.RerF import fastPredict, fastRerF, fastPredictPost


def test_binning():
    datafile = "packedForest/res/iris.csv"
    label_col = 4
    forest = fastRerF(
        CSVFile=datafile,
        Ycolumn=label_col,
        forestType="rfBase",
        trees=50, 
        minParent=1,
        maxDepth=None,
        binMin = 8,
        binSize = 64,
        numCores=1,
        mtry=2,
        seed=30,
    )

    params = get_params(forest)

    assert params["binMin"] == '8'
    assert params["binSize"] == '64'


