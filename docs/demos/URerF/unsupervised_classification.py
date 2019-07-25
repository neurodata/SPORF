#%%
from sklearn import datasets
from rerf.urerf import UnsupervisedRandomForest
from sklearn.cluster import AgglomerativeClustering
import matplotlib.pyplot as plt


#%%
# Load dataset
iris = datasets.load_iris()


#%%
X = iris.data


#%%
clf = UnsupervisedRandomForest(
    projection_matrix="Base", n_estimators=100, min_parent=10
)


#%%
clf.fit(X)
sim_mat = clf.transform()


#%%
plt.imshow(sim_mat)
plt.savefig("iris_heatmap.png")
sim_mat

#%%
cluster = AgglomerativeClustering(n_clusters=3, affinity="euclidean", linkage="ward")
cluster.fit_predict(sim_mat)

#%%
iris.target
