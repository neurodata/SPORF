from rerf.RerF import fastRerF, fastPredict, retSimMat
import numpy as np
from multiprocessing import cpu_count
from scipy.sparse import *
from scipy.sparse.linalg import svds, eigs
import matplotlib.pyplot as plt
import timeit

datatype = "iris"
from sklearn import *
from sklearn.cluster import AgglomerativeClustering

# datatype = "mnist"
ns = [1000]
for n in ns:
    if datatype == "iris":
        datafile = "../packedForest/res/iris.csv"
        label_col = 4
    elif datatype == "mnist":
        datafile = "../packedForest/res/mnist.csv"
        label_col = 0

    print("loading data...")
    X = np.genfromtxt(datafile, delimiter=",")
    print("data loaded")

    if datatype == "iris":
        feat_data = X[:, 0:4]  # iris
    elif datatype == "mnist":
        feat_data = X[:, 1:]  # mnist

    # labels = X[:, label_col]
    labels = np.zeros(X.shape[0])
    labels[0] = 1

    # forest = fastRerF(
    #     CSVFile=datafile,
    #     Ycolumn=label_col,
    #     forestType="binnedBaseRerF",
    #     trees=500,
    #     numCores=cpu_count() - 1,
    # )
    forest = fastRerF(
        X=feat_data,
        Y=labels,
        forestType="urf",
        # forestType="rfBase",
        trees=100,
        numCores=1,
        minParent=1,
    )

    start = timeit.timeit()
    pairMat = retSimMat(forest)
    end = timeit.timeit()
    print(end - start)

tupList = []
dataCounts = []

for key, value in pairMat.items():
    tupList.append(key)
    dataCounts.append(value)

row = []
col = []

for tup in tupList:
    row.append(tup[0])
    col.append(tup[1])

X_new = feat_data
sparse_mat = csr_matrix(
    (dataCounts, (row, col)), shape=(X_new.shape[0], X_new.shape[0])
)
sparse_mat = sparse_mat.astype(float)
simMatDense = sparse_mat.todense()
print(simMatDense)
# np.save("simpyrerf.npy", simMatDense)
plt.imshow(simMatDense, interpolation="none")
plt.savefig("heatmap.png")
