import multiprocessing
from warnings import warn

import numpy as np
from sklearn.base import BaseEstimator, ClassifierMixin
from sklearn.utils.multiclass import unique_labels
from sklearn.utils.validation import check_array, check_is_fitted, check_X_y
from sklearn.exceptions import DataConversionWarning

import pyfp


class rerfClassifier(BaseEstimator, ClassifierMixin):
    """A random forest classifier.

    Supports both Random Forest, developed by Breiman (2001) [#Breiman]_, as well as 
    Randomer Forest or Random Projection Forests (RerF) developed by 
    Tomita et al. (2016) [#Tomita]_.

    The difference between the two algorithms is where the random linear 
    combinations occur: Random Forest combines features at the tree level 
    whereas RerF combines features at the node level.

    There are two new parameters to be aware of:
        
        - ``projection_matrix``
        - ``feature_combinations``
    
    For more information, see :ref:`Parameters <rerfClassifier_params>`.

    References
    ----------
    .. [#Breiman] Breiman (2001).
        https://doi.org/10.1023/A:1010933404324

    .. [#Tomita] Tomita et al. (2016).
        https://arxiv.org/abs/1506.03410

    .. _`rerfClassifier_params`:
    Parameters
    ----------
    projection_matrix : str, optional (default: "RerF")
        The type of random combination of features to use: either "RerF" or
        "Base".  See Tomita et al. (2016) [#Tomita]_ for further details. 
    n_estimators : int, optional (default: 500)
        Number of trees in forest.

        Note: This differs from scikit-learn's default of 100.
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
    >>> from rerfClassifier import rerfClassifier
    >>> from sklearn.datasets import make_classification

    >>> X, y = make_classification(
    ...    n_samples=1000,
    ...    n_features=4,
    ...    n_informative=2,
    ...    n_redundant=0,
    ...    random_state=0,
    ...    shuffle=False,
    ... )
    >>> clf = rerfClassifier(n_estimators=100, max_depth=2, random_state=0)
    >>> clf.fit(X, y)
    starting tree 1
    max depth: 2
    avg leaf node depth: 1.9899
    num leaf nodes: 396
    rerfClassifier(feature_combinations=1.5, max_depth=2, max_features='auto',
            min_parent=1, n_estimators=100, n_jobs=None,
            projection_matrix='RerF', random_state=0)
    >>> print(clf.predict([[0, 0, 0, 0]]))
    [1]
    >>> print(clf.predict_proba([[0, 0, 0, 0]]))
    [[0.2 0.8]]

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
        y : array-like, 1D numpy array
            Labels
        
        Returns
        -------
        self : object
        
        """

        # Check that X and y have correct shape
        X, y = check_X_y(X, y)

        y = np.atleast_1d(y)
        if y.ndim == 2 and y.shape[1] == 1:
            warn(
                "A column-vector y was passed when a 1d array was"
                " expected. Please change the shape of y to "
                "(n_samples,), for example using ravel().",
                DataConversionWarning,
                stacklevel=2,
            )

        if y.ndim == 1:
            # reshape is necessary to preserve the data contiguity against vs
            # [:, np.newaxis] that does not.
            y = np.reshape(y, (-1, 1))

        # setup the forest's parameters
        self.forest_ = pyfp.fpForest()

        if self.projection_matrix == "RerF":
            forestType = "binnedBaseTern"
        elif self.projection_matrix == "Base":
            forestType = "binnedBase"
        else:
            raise ValueError("Incorrect projection matrix")
        self.forest_.setParameter("forestType", forestType)

        self.forest_.setParameter("numTreesInForest", self.n_estimators)

        # if max_depth is not set, C++ sets to maximum integer size
        if self.max_depth is not None:
            self.forest_.setParameter("maxDepth", self.max_depth)

        self.forest_.setParameter("minParent", self.min_parent)

        self.forest_.setParameter("mtryMult", self.feature_combinations)

        if self.n_jobs is None:
            self.n_jobs_ = 1
        elif self.n_jobs == -1:
            self.n_jobs_ = multiprocessing.cpu_count()
        self.forest_.setParameter("numCores", self.n_jobs_)

        if self.random_state is None:
            self.random_state_ = np.random.randint(1, 1000000)
        else:
            self.random_state_ = self.random_state
        self.forest_.setParameter("seed", self.random_state_)

        num_obs = len(y)
        num_features = X.shape[1]

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

        # Explicitly setting for numpy input
        self.forest_.setParameter("useRowMajor", 1)

        self.forest_._growForestnumpy(X, y, num_obs, num_features)

        # Store the classes seen during fit
        self.classes_ = unique_labels(y)

        self.X_ = X
        self.y_ = y
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

        # Check is fit had been called
        check_is_fitted(self, ["X_", "y_"])

        # Input validation
        X = check_array(X)

        if X.ndim == 1:
            predictions = self.forest_._predict(X.tolist())
        else:
            predictions = self.forest_._predict_numpy(X)
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

        # Check is fit had been called
        check_is_fitted(self, ["X_", "y_"])

        # Input validation
        X = check_array(X)

        if X.ndim == 1:
            y = self.forest_._predict_post(X.tolist())
            y_prob = [p / sum(y) for p in y]
        else:
            y = self.forest_._predict_post_array(X)
            y_arr = np.asarray(y)
            y_prob = y_arr / y_arr.sum(1)[:, None]
        return y_prob

    def predict_log_proba(self, X):
        """Predict class log-probabilities for X.
        The predicted class log-probabilities of an input sample is computed as
        the log of the mean predicted class probabilities of the trees in the
        forest.

        Parameters
        ----------
        X : array-like or sparse matrix of shape = [n_samples, n_features]
            The input samples. Internally, its dtype will be converted to
            ``dtype=np.float32``.
        
        Returns
        -------
        p : array of shape = [n_samples, n_classes], or a list of n_outputs
            such arrays if n_outputs > 1.
            The class probabilities of the input samples. The order of the
            classes corresponds to that in the attribute `classes_`.
        """
        proba = self.predict_proba(X)

        return np.log(proba)
