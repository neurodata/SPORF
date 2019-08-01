import multiprocessing

import numpy as np
from scipy.sparse import csr_matrix
from sklearn.base import BaseEstimator
from sklearn.utils import check_array
from sklearn.utils.validation import check_is_fitted

import pyfp


class UnsupervisedRandomForest(BaseEstimator):
    """Unsupervised random(er) forest

    Supports both Random Forest, developed by Breiman (2001) [#Breiman]_, as well as
    Randomer Forest or Random Projection Forests (RerF) developed by
    Tomita et al. (2016) [#Tomita]_.

    The difference between the two algorithms is where the random linear
    combinations occur: Random Forest combines features at the tree level
    whereas RerF combines features at the node level.

    In addition to the normal RandomForestClassifier parameters, there are
    two parameters to be aware of:

        - ``projection_matrix``
        - ``feature_combinations``

    Parameters
    ----------
    projection_matrix : str, optional (default: "RerF")
        The random combination of features to use: either "RerF", "Base".
        "RerF" randomly combines features for each `mtry`. Base is our
        implementation of Random Forest. "S-RerF" is structured RerF,
        combining multiple features together in random patches.
        See Tomita et al. (2016) [#Tomita]_ for further details.
    n_estimators : int, optional (default: 100)
        Number of trees in forest.
    max_depth : int or None, optional (default=None)
        The maximum depth of the tree. If None, then nodes are expanded
        until all leaves are pure or until all leaves contain less than
        min_samples_split samples.
    min_samples_split : int, optional (default: "auto")
        The minimum splittable node size.  A node size < ``min_samples_split``
        will be a leaf node.  Note: other implementations called `min.parent`
        or `minParent`

        - If "auto", then ``min_samples_split=sqrt(num_obs)``
        - If int, then consider ``min_samples_split`` at each split.

    max_features : int, float, string, or None, optional (default="auto")
        The number of features or feature combinations to consider when
        looking for the best split.  Note: also called `mtry` or `d`.

        - If int, then consider ``max_features`` features or feature combinations at each split.
        - If float, then `max_features` is a fraction and ``int(max_features * n_features)`` features are considered at each split.
        - If "auto", then ``max_features=sqrt(n_features)``.
        - If "sqrt", then ``max_features=sqrt(n_features)`` (same as "auto").
        - If "log2", then ``max_features=log2(n_features)``.
        - If None, then ``max_features=n_features``.

    feature_combinations : float, optional (default: "auto")
        Average number of features combined to form a new feature when
        using "RerF." Otherwise, ignored.

        - If int or float, then ``feature_combinations`` is average number of features to combine for each ``max_features`` to try.
        - If "auto", then ``feature_combinations=n_features``.
        - If "sqrt", then ``feature_combinations=sqrt(n_features)`` (same as "auto").
        - If "log2", then ``feature_combinations=log2(n_features)``.
        - If None, then ``feature_combinations=n_features``.

    n_jobs : int or None, optional (default=None)
        The number of jobs to run in parallel for both `fit` and `predict`.
        ``None`` means 1. ``-1`` means use all processors.
    random_state : int or None, optional (default=None)
        Random seed to use. If None, set seed to ``np.random.randint(1, 1000000)``.

    Returns
    -------

    Examples
    --------
    >>> from matplotlib import pyplot as plt
    >>> from sklearn.cluster import AgglomerativeClustering
    >>> from sklearn.datasets import make_classification
    >>> from sklearn.metrics import adjusted_rand_score
    >>> from rerf.urerf import UnsupervisedRandomForest

    >>> X, y = make_classification(
    ...    n_samples=1000,
    ...    n_features=4,
    ...    n_informative=2,
    ...    n_redundant=0,
    ...    random_state=0,
    ...    shuffle=False,
    ... )
    >>> clf = UnsupervisedRandomForest(n_estimators=100, random_state=0)
    >>> clf.fit(X)
    >>> sim_mat = clf.transform()
    >>> plt.imshow(sim_mat)
    >>> cluster = AgglomerativeClustering(n_clusters=2)
    >>> predict_labels = cluster.fit_predict(sim_mat)
    >>> score = adjusted_rand_score(y, predict_labels)
    >>> print(score)
    0.7601439767776818

    Notes
    -----

    """

    def __init__(
        self,
        projection_matrix="RerF",
        n_estimators=100,
        max_depth=None,
        min_samples_split="auto",
        max_features="auto",
        feature_combinations="auto",
        n_jobs=None,
        random_state=None,
    ):
        self.projection_matrix = projection_matrix
        self.n_estimators = n_estimators
        self.max_depth = max_depth
        self.min_samples_split = min_samples_split
        self.max_features = max_features
        self.feature_combinations = feature_combinations
        self.n_jobs = n_jobs
        self.random_state = random_state

    def fit(self, X, y=None):
        """Fit estimator.
        Parameters
        ----------
        X : array-like, shape=(n_samples, n_features)
            Input data.  Rows are observations and columns are features.

        Returns
        -------
        self : object

        """

        # Check that X and y have correct shape
        X = check_array(X, accept_sparse=["csc"])
        num_features = X.shape[1]
        num_obs = X.shape[0]

        # setup the forest's parameters
        self.forest_ = pyfp.fpForest()

        if self.projection_matrix == "Base":
            forestType = "urf"
        elif self.projection_matrix == "RerF":
            forestType = "urerf"
        else:
            raise ValueError("Incorrect projection matrix")
        self.forest_.setParameter("forestType", forestType)

        self.forest_.setParameter("numTreesInForest", self.n_estimators)

        # if max_depth is not set, C++ sets to maximum integer size
        if self.max_depth is not None:
            self.forest_.setParameter("maxDepth", self.max_depth)

        if self.min_samples_split == "auto":
            self.min_samples_split_ = round(num_obs ** (1 / 2))
        else:
            self.min_samples_split_ = self.min_samples_split
        self.forest_.setParameter("minParent", self.min_samples_split_)

        if self.n_jobs is None:
            self.n_jobs_ = 1
        elif self.n_jobs == -1:
            self.n_jobs_ = multiprocessing.cpu_count()
        else:
            self.n_jobs_ = self.n_jobs
        self.forest_.setParameter("numCores", self.n_jobs_)

        if self.random_state is None:
            self.random_state_ = np.random.randint(1, 1000000)
        else:
            self.random_state_ = self.random_state
        self.forest_.setParameter("seed", self.random_state_)

        # need to set mtry here (using max_features and calc num_features):
        if self.max_features in ("auto", "sqrt"):
            self.mtry_ = int(num_features ** (1 / 2))
        elif self.max_features is None:
            self.mtry_ = num_features
        elif self.max_features == "log2":
            self.mtry_ = int(np.log2(num_features))
        elif isinstance(self.max_features, int):
            self.mtry_ = self.max_features
        elif isinstance(self.max_features, float) and 0 <= self.max_features <= 1:
            self.mtry_ = int(self.max_features * num_features)
        else:
            raise ValueError("max_features has unexpected value")
        self.forest_.setParameter("mtry", self.mtry_)

        if self.feature_combinations == "auto":
            self.feature_combinations_ = num_features
        elif self.feature_combinations == "sqrt":
            self.feature_combinations_ = num_features ** (1 / 2)
        elif self.feature_combinations is None:
            self.feature_combinations_ = num_features
        elif self.feature_combinations == "log2":
            self.feature_combinations_ = np.log2(num_features)
        elif isinstance(self.feature_combinations, (int, float)):
            self.feature_combinations_ = self.feature_combinations
        else:
            raise ValueError("feature_combinations_ has unexpected value")
        self.forest_.setParameter("mtryMult", self.feature_combinations_)

        # Explicitly setting for numpy input
        self.forest_.setParameter("useRowMajor", 1)

        # y will be ignored
        y = np.zeros(num_obs)
        self.forest_._growForestnumpy(X, y, num_obs, num_features)

        self.X_ = X

        return self

    def transform(self, return_sparse=False):
        """Transform dataset into an affinity matrix / similarity matrix.

        Parameters
        ----------

        Returns
        -------
        affinity_matrix : sparse matrix, shape=(n_samples, n_samples)
        """

        check_is_fitted(self, "forest_")

        pair_mat = self.forest_._return_pair_mat()

        sparse_mat = pair_mat_to_sparse(pair_mat, self.X_.shape[0], self.n_estimators)

        if return_sparse:
            return sparse_mat
        else:
            return sparse_mat.toarray()


def pair_mat_to_sparse(pair_mat, n_obs, n_estimators):
    i = [ij[0] for ij in pair_mat.keys()]
    j = [ij[1] for ij in pair_mat.keys()]
    data = [d / n_estimators for d in pair_mat.values()]
    sparse_mat = csr_matrix((data, (i, j)), shape=(n_obs, n_obs), dtype=float)

    return sparse_mat
