from scipy.sparse import csc_matrix
from sklearn.cluster import AgglomerativeClustering

# from sklearn.datasets import make_classification
from sklearn.datasets.samples_generator import make_blobs

from rerf.urerf import UnsupervisedRandomForest


def test_urerf():
    # X, y = make_classification(
    #     n_samples=100,
    #     n_features=4,
    #     n_informative=2,
    #     n_redundant=0,
    #     random_state=0,
    #     shuffle=False,
    # )
    n_samples = 100
    n_classes = 2

    X, y = make_blobs(
        n_samples=n_samples, centers=n_classes, n_features=2, random_state=2 ** 4
    )

    clf = UnsupervisedRandomForest(n_estimators=100, max_depth=None, random_state=0)
    clf.fit(X)
    sim_mat = clf.transform()

    i = [ij[0] for ij in sim_mat.keys()]
    j = [ij[1] for ij in sim_mat.keys()]
    sim_mat_full = csc_matrix((list(sim_mat.values()), (i, j))).toarray()

    clustering = AgglomerativeClustering(n_clusters=n_classes).fit(sim_mat_full)
