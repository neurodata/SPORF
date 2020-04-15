import numpy as np
from graspy.simulations import sbm

def get_2class_sbms(n=1000,p=0.5):
    ns = [50,50]
    p1 = [[0.1,0.05],[0.05,0.1]]
    p2 = [[0.05,0.1],[0.1,0.05]]

    X = []
    y = []
    for _ in range(int(n*p)):
        X.append(sbm(ns, p1))
        y.append(0)

    for _ in range(int(n*(1-p))):
        X.append(sbm(ns, p2))
        y.append(1)

    idxs = list(range(len(y)))
    np.random.shuffle(idxs)
    return(np.asarray(X)[idxs], np.asarray(y)[idxs])