import numpy as np
import pytest
from sklearn.cluster import AgglomerativeClustering
from sklearn.datasets.samples_generator import make_blobs
from sklearn.metrics import adjusted_rand_score

from rerf.urerf import UnsupervisedRandomForest, pair_mat_to_sparse


def test_pair_mat_to_sparse():
    n_est = 3
    pair_mat_data = {(0, 0): 1, (0, 1): 2, (1, 0): 2, (1, 1): 3}
    sparse_mat = pair_mat_to_sparse(pair_mat_data, 2, n_est)
    dense_mat = sparse_mat.toarray()

    expect_mat = np.array([[1, 2], [2, 3]], dtype=float) / n_est

    assert np.array_equal(dense_mat, expect_mat)


@pytest.mark.parametrize("projection_matrix", ("RerF", "Base"))
def test_urerf(projection_matrix):
    n_samples = 100
    n_classes = 2
    X, y = make_blobs(
        n_samples=n_samples, centers=n_classes, n_features=2, random_state=2 ** 4
    )

    clf = UnsupervisedRandomForest(projection_matrix=projection_matrix)
    clf.fit(X)
    sim_mat = clf.transform()

    assert np.array_equal(sim_mat.diagonal(), np.ones(n_samples))

    cluster = AgglomerativeClustering(n_clusters=n_classes).fit(sim_mat)
    predict_labels = cluster.fit_predict(sim_mat)
    score = adjusted_rand_score(y, predict_labels)
    assert score > 0.9
    
    // adding K-Means Clustering for comparison.
    cluster = KMeans(n_clusters= i)
    predict_labels = cluster.fit_predict(sim_mat)
    score = adjusted_rand_score(Y, predict_labels)
    assert score > 0.9
    
    
    
