#%%
from sklearn import datasets
import numpy as np
from rerf.urerf import UnsupervisedRandomForest
from sklearn.cluster import AgglomerativeClustering
from scipy.sparse import csc_matrix, csr_matrix
import matplotlib.pyplot as plt


#%%
# Load dataset
iris = datasets.load_iris()


#%%
X = iris.data


#%%
clf = UnsupervisedRandomForest(
    projection_matrix="RerF", n_estimators=100, min_parent=10
)

#%%
def sim_mat_toarray(pairMat, X):
    tupList = []
    dataCounts = []
    for key, value in pairMat.items():
        tupList.append(key)
        dataCounts.append(value)
        tupList.append((key[1], key[0]))
        dataCounts.append(value)

    row = []
    col = []
    for tup in tupList:
        row.append(tup[0])
        col.append(tup[1])

    X_new = X
    sparse_mat = csr_matrix(
        (dataCounts, (row, col)), shape=(X_new.shape[0], X_new.shape[0])
    )
    return sparse_mat.toarray()


#%%
clf.fit(X)
sim_mat = clf.transform()
A = sim_mat_toarray(sim_mat, X)

# i = [ij[1] for ij in sim_mat.keys()]
# j = [ij[0] for ij in sim_mat.keys()]
# A = csc_matrix(
#     (list(sim_mat.values()), (i, j)), shape=(X.shape[0], X.shape[0])
# ).toarray()


#%%
plt.imshow(A)

#%%
cluster = AgglomerativeClustering(n_clusters=3, affinity="euclidean", linkage="ward")
cluster.fit_predict(A)

#%%
iris.target
