from rerf.rerfClassifier import rerfClassifier
import numpy as np
import io
from contextlib import redirect_stdout

def get_forest_params(forest):
    params = {}

    f = io.StringIO()
    with redirect_stdout(f):
        forest.printParameters()
    out = f.getvalue()

    for line in out.splitlines():
        A = line.split(" -> ")
        params[A[0]] = A[1]

    return params

X = np.random.normal(0,1,(10,10))
Y = np.asarray([0]*5+[1]*5)

clf = rerfClassifier(n_estimators=5)
clf = clf.fit(X,Y)
print(get_forest_params(clf.forest_))