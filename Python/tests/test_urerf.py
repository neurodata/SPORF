import numpy as np
from scipy.sparse import csc_matrix
from sklearn.cluster import AgglomerativeClustering

# from sklearn.datasets import make_classification
from sklearn.datasets.samples_generator import make_blobs

from rerf.urerf import UnsupervisedRandomForest, pair_mat_to_sparse


def test_pair_mat_to_sparse():
    n_est = 3
    pair_mat_data = {(0, 0): 1, (0, 1): 2, (1, 0): 2, (1, 1): 3}
    sparse_mat = pair_mat_to_sparse(pair_mat_data, 2, n_est)
    dense_mat = sparse_mat.toarray()

    expect_mat = np.array([[1, 2], [2, 3]], dtype=float) / n_est

    assert np.array_equal(dense_mat, expect_mat)


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

    clustering = AgglomerativeClustering(n_clusters=n_classes).fit(sim_mat)
