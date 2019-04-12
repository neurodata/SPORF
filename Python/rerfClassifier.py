import multiprocessing

import numpy as np

import pyfp


class rerfClassifier:
    """A random forest classifier.

    Supports both Random Forest, developed by Breiman (2001) [#Breiman]_, as well as 
    Randomer Forest or Random Projection Forests (RerF) developed by 
    Tomita et al. (2016) [#Tomita]_.

    The difference between the two algorithms is where the random linear 
    combinations occur: Random Forest combines features at the tree level 
    whereas RerF combines features at the node level.
    
    References
    ----------
    .. [#Breiman] Breiman (2001).
        https://doi.org/10.1023/A:1010933404324

    .. [#Tomita] Tomita et al. (2016).
        https://arxiv.org/abs/1506.03410

    Parameters
    ----------
    projection_matrix : str, optional (default: "RerF")
        The type of random combination of features to use: "RerF", "Base"
        See Tomita et al. (2016) [#Tomita]_ for further details. 
    n_estimators : int, optional (default: 500)
        Number of trees in forest.
    max_depth : int or None, optional (default=None)
        The maximum depth of the tree. If None, then nodes are expanded 
        until all leaves are pure or until all leaves contain less than min_samples_split samples.
    min_samples_split : int, optional (default: 1)
        The minimum splittable node size.  A node size < ``min_samples_split`` 
        will be a leaf node.  Note: also called `min.parent` or `minParent`
    max_features : int, float, string, or None, optional (default="auto")
        The number of features or feature combinations to consider when 
        looking for the best split.  Note: also called `mtry` or `d`.

        - If int, then consider ``max_features`` features or feature combinations at each split.
        - If float, then `max_features` is a fraction and ``int(max_features * n_features)`` features are considered at each split.
        - If "auto", then ``max_features=sqrt(n_features)``.
        - If "sqrt", then ``max_features=sqrt(n_features)`` (same as "auto").
        - If "log2", then ``max_features=log2(n_features)``.
        - If None, then ``max_features=n_features``.

    feature_combinations : float, optional (default: 1.5)
        Average number of features combined to form a new feature when
        using "RerF."  Otherwise, ignored.
    n_jobs : int or None, optional (default=None)
        The number of jobs to run in parallel for both `fit` and `predict`.
        ``None`` means 1. ``-1`` means use all processors. 
    random_state : int or None, optional (default=None)
        Random seed to use. If None, set seed to ``np.random.randint(1, 1000000)``.

    Returns
    -------
    
    Examples
    --------

    Notes
    -----

    """

    def __init__(
        self,
        projection_matrix="RerF",
        n_estimators=500,
        max_depth=None,
        min_parent=1,
        max_features="auto",
        feature_combinations=1.5,
        n_jobs=None,
        random_state=None,
    ):
        # setup the forest's parameters
        self.forest = pyfp.fpForest()

        if projection_matrix == "RerF":
            forestType = "binnedBaseTern"
        elif projection_matrix == "Base":
            forestType = "binnedBase"
        self.forest.setParameter("forestType", forestType)

        self.forest.setParameter("numTreesInForest", n_estimators)

        # if max_depth is not set, C++ sets to maximum integer size
        if max_depth is not None:
            self.forest.setParameter("maxDepth", max_depth)

        self.forest.setParameter("minParent", min_parent)

        self.forest.setParameter("mtryMult", feature_combinations)

        if n_jobs is None:
            n_jobs = 1
        elif n_jobs == -1:
            n_jobs = multiprocessing.cpu_count()
        self.forest.setParameter("numCores", n_jobs)

        if random_state is None:
            random_state = np.random.randint(1, 1000000)
        self.forest.setParameter("seed", random_state)

        self.projection_matrix = projection_matrix
        self.n_estimators = n_estimators
        self.max_depth = max_depth
        self.min_parent = min_parent
        self.max_features = max_features
        self.feature_combinations = feature_combinations
        self.n_jobs = n_jobs
        self.random_state = random_state

    def fit(self, X, y):
        """Fit estimator.

        Parameters
        ----------
        X : array-like, shape=(n_samples, n_features)
            Input data.  Rows are observations and columns are features.
        y : list, 1D numpy array
            Labels
        
        Returns
        -------
        self : object
        
        """

        num_obs = len(y)
        num_features = X.shape[1]

        # need to set mtry here (using self.max_features and calc num_features):
        if self.max_features in ("auto", "sqrt"):
            mtry = int(num_features ** (1 / 2))
        elif self.max_features is None:
            mtry = num_features
        elif self.max_features == "log2":
            mtry = int(np.log2(num_features))
        elif isinstance(self.max_features, int):
            mtry = self.max_features
        elif isinstance(self.max_features, float) and 0 <= self.max_features <= 1:
            mtry = int(self.max_features * num_features)
        else:
            raise ValueError("max_features has unexpected value")
        self.forest.setParameter("mtry", mtry)

        self.forest._growForestnumpy(X, y, num_obs, num_features)
        return self

    def predict(self, X):
        pass

    def predict_proba(self, X):
        pass

    def get_params(self):
        pass
