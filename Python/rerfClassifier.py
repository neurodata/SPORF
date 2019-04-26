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
        until all leaves are pure or until all leaves contain less than 
        min_samples_split samples.
    min_samples_split : int, optional (default: 1)
        The minimum splittable node size.  A node size < ``min_samples_split`` 
        will be a leaf node.  Note: other implementations called `min.parent`
        or `minParent`
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

        # setup the forest's parameters
        self.forest = pyfp.fpForest()

        if self.projection_matrix == "RerF":
            forestType = "binnedBaseTern"
        elif self.projection_matrix == "Base":
            forestType = "binnedBase"
        else:
            raise ValueError("Incorrect projection matrix")
        self.forest.setParameter("forestType", forestType)

        self.forest.setParameter("numTreesInForest", self.n_estimators)

        # if max_depth is not set, C++ sets to maximum integer size
        if self.max_depth is not None:
            self.forest.setParameter("maxDepth", self.max_depth)

        self.forest.setParameter("minParent", self.min_parent)

        self.forest.setParameter("mtryMult", self.feature_combinations)

        if self.n_jobs is None:
            self.n_jobs = 1
        elif self.n_jobs == -1:
            self.n_jobs = multiprocessing.cpu_count()
        self.forest.setParameter("numCores", self.n_jobs)

        if self.random_state is None:
            self.random_state = np.random.randint(1, 1000000)
        self.forest.setParameter("seed", self.random_state)

        num_obs = len(y)
        num_features = X.shape[1]

        # need to set mtry here (using max_features and calc num_features):
        if self.max_features in ("auto", "sqrt"):
            self.max_features = int(num_features ** (1 / 2))
        elif self.max_features is None:
            self.max_features = num_features
        elif self.max_features == "log2":
            self.max_features = int(np.log2(num_features))
        elif isinstance(self.max_features, int):
            self.max_features = self.max_features
        elif isinstance(self.max_features, float) and 0 <= self.max_features <= 1:
            self.max_features = int(self.max_features * num_features)
        else:
            raise ValueError("max_features has unexpected value")
        self.forest.setParameter("mtry", self.max_features)

        # Explicitly setting for numpy input
        self.forest.setParameter("useRowMajor", 1)

        self.forest._growForestnumpy(X, y, num_obs, num_features)

        return self

    def predict(self, X):
        """Predict class for X.
        
        Parameters
        ----------
        X : array_like of shape [nsamples, n_features]
            The input samples.  If more than 1 row, run multiple predictions.
        Returns
        -------
        y : int, list of int
            Returns the class of prediction (int) or predictions (list) 
            depending on input parameters.
        """

        X = np.asarray(X)

        if X.ndim == 1:
            predictions = self.forest._predict(X.tolist())
        else:
            predictions = self.forest._predict_numpy(X)
        return predictions

    def predict_proba(self, X):
        """Predict class probabilities for X.
        The predicted class probabilities of an input sample are computed as
        the mean predicted class of the trees in the forest.
        
        Parameters
        ----------
        X : array_like of shape [nsamples, n_features]
            The input samples.  If more than 1 row, run multiple predictions.
        Returns
        -------
        p : array of shape = [n_samples, n_classes]
            The class probabilities of the input samples. The order of the
            classes corresponds to that in the attribute `classes_`.
        """

        X = np.asarray(X)

        if X.ndim == 1:
            y = forest._predict_post(X.tolist())
            y_prob = [p / sum(y) for p in y]
        else:
            y = forest._predict_post_array(X)
            y_arr = np.asarray(y)
            y_prob = y_arr / y_arr.sum(1)[:, None]
        return y_prob

    def get_params(self):
        pass
