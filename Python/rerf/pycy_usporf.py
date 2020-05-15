import numpy as np
import random as rn
import cy_usporf  # Utilities functions

class UForest(object):
    """ UForest: USPORF forest
    Creates an iForest object. This object holds the data as well as
    the trained trees (iTree objects).
    
    Note: download `cy_usporf.pyx` and `setup.py` into your local
    computer and compile them to generate and generate `.C` and `.so`
    before using this module

    Parameters
    ----------
    n_samples: int
        Size of the dataset.
    max_sample: int
        Size of the sample to be used for tree creation.
    Trees: list
        A list of tree objects.
    max_depth: int
        Maximum depth a tree can have.
    d: int
        number of features in limit sparse matrix `A`
    
    References
    ----------
    .. [#Meghana] Meghana et al (2019).
        https://arxiv.org/abs/1907.02844

    .. [#Fei] Fei Tony et al. (2009).
        https://arxiv.org/abs/1506.03410

    Methods
    -------
    compute_paths(X_in)
        Computes the anomaly score for data X_in
    """
    def __init__(self, n_estimators, max_samples=None,
                 max_depth=None, d=None, Lambda=1/20):
        self.n_estimators = n_estimators
        self.max_samples = max_samples
        self.max_depth = max_depth
        self.d = d
        self.Lambda = Lambda
        # 1/20: prob of +1, -1 element in sparse projected matrix `A`

    def fit(self, X, y=None):
        self.Trees = []
        self.X_ = X
        n_samples = X.shape[0]
        dim = X.shape[1]

        if self.max_samples is None:
            self.max_samples = min(256, n_samples)
        if self.max_depth is None:
            self.max_depth = int(np.ceil(np.log2(self.max_samples)))
        if self.d is None:
            self.d = int(np.sqrt(dim))
        self.c = cy_usporf.c_factor(self.max_samples)
        for i in range(self.n_estimators):  # Ensemble of iTrees (the forest).
            ix = rn.sample(range(n_samples), self.max_samples)  # subset
            X_p = X[ix]
            self.Trees.append(iTree(X_p, 0, self.max_depth, d=self.d,
                                    Lambda=self.Lambda))
        return self

    def compute_paths(self, X_in=None):
        """ Compute paths(X_in = None)

        Compute anomaly scores for all data points in a dataset X_in
        Parameters
        ----------
        X_in : list of list of floats
            Data to be scored. iForest.Trees are used for computing
            the depth reached in each tree by each data point.

        Returns
        -------
        float
            Anomaly score for a given data point. The higher score,
            the more anomaly.
        """
        if X_in is None:
            X_in = self.X_
        S = np.zeros(len(X_in))
        for i in np.arange(len(X_in)):
            h_temp = 0
            for j in range(self.n_estimators):
                h_temp += PathFactor(X_in[i], self.Trees[j]).path*1.0
            Eh = h_temp/self.n_estimators
            # Average of path length travelled by the point in all trees.
            S[i] = 2.0**(-Eh/self.c)  # higher score, more anomaly
        return S


class Node(object):
    """Elements stored in tree node 
    A single node from each tree (each iTree object). Nodes containe
    information on hyperplanes used for data division, date to be passed
    to left and right nodes, whether they are external or internal nodes.

    Attributes
    ----------
    e: int
        Depth of the tree to which the node belongs.
    size: int
        Size of the dataset present at the node.
    d: int
        Dimension of the projected space
    X: list
        Data at the node.
    Aj: list
        project matrix column j
    splitPoint: float
        splitPoint/ split threshold
    left: Node object
        Left child node.
    right: Node object
        Right child node.
    node_type: str
        The type of the node: 'LeafNode', 'inNode'.
    """
    def __init__(self, X, d, Aj, splitPoint, e, left, right, node_type=''):
        self.e = e
        self.size = len(X)
        self.X = X
        self.d = d
        self.Aj = Aj
        self.splitPoint = splitPoint
        self.left = left
        self.right = right
        self.ntype = node_type


class iTree(object):  # USPORF's algo 1
    """iTree: a single USPORF tree
    A single tree in the forest that is build using a unique subsample.

    Attributes
    ----------
    X: list
        Data present at the root node of this tree.
    e: int
        Depth of a current node
    max_depth:
        length limit of the tree node
    d: int
        number of feature in a projected matrix `A`
    Lambda: float
        ratio of non-zero lement (-1 and 1) in a projected matrix `A`

    Methods
    -------
    make_tree(X, e)
        Builds the tree recursively from a given node. Returns a Node object.
    """
    def __init__(self, X, e, max_depth, d, Lambda):
        self.max_depth = max_depth
        self.Lambda = Lambda
        self.d = d
        self.size = X.shape[0]  # n: number of sample in the tree root
        self.min_samples_split = int(np.sqrt(self.size))  # default
        self.dim = X.shape[1]  # p: number of original feature
        self.d_list = np.arange(d, dtype='int')  # [0,1,2,...,'d'-1] #?
        self.Aj = None
        self.splitPoint = None
        self.LeafNodes = 0  # number of LeafNode / leaf node
        self.root = self.make_tree(X, e)  # Create a tree with root node.

    def make_tree(self, X, e):
        """make_tree(X, e, l)
        Builds the tree recursively from a given node. Returns a Node object.

        Parameters
        ----------
        X: list of list of floats
            Subsample of training data. |X| = iForest.max_samples.
            Might not equal to the fist `X` in class iTree():
        e : int
            Depth of the tree. e <= max_depth.
        max_depth : int
            The maximum depth the tree can reach before its creation
            is terminated. Integer.
        Returns
        -------
        Node object
        """
        if e >= self.max_depth or len(X) <= self.min_samples_split:
            # LeafNode condition
            left = None
            right = None
            self.LeafNodes += 1
            return Node(X, self.d, self.Aj, self.splitPoint, e,
                        left, right, node_type='LeafNode')

        else:   # Build a tree recursively
            A = cy_usporf.projectA(self.dim, self.d, self.Lambda)
            XA = X @ A  # [n*d] array
            min_t = np.inf
            for j in self.d_list:  # dimension
                (midpt, t_) = cy_usporf.FastBIC(XA[:, j])
                if t_ < min_t:
                    bestDim = j
                    splitPoint = midpt

            w = XA[:, bestDim] < splitPoint  # spliting criteria
            return Node(X, self.d, A[:, bestDim], splitPoint, e,
                        left=self.make_tree(X[w], e+1),
                        right=self.make_tree(X[~w], e+1),
                        node_type='inNode')


class PathFactor(object):
    """ Path length finder
    Given a single tree (iTree objext) and a data point x = [x1,x2,...,xn],
    compute the legth of the path traversed by the point on the tree when
    it reaches an external node. See Fei Tony et al. (2009) [#Fei]_ for
    further details

    Attributes
    ----------
    path_list: list
        A list of strings 'L' or 'R' which traces the path
        a data point travels down a tree.
    x: list
        A single data point, which is represented as a list of floats.
    e: int
        The depth of a given node in the tree.
    itree: iTree object
        A single tree

    Methods
    -------
    find_path(T)
        Given a tree, it finds the path a single data points takes.
    """
    def __init__(self, x, itree):
        self.path_list = []
        self.x = x
        self.e = 0
        self.path = self.find_path(itree.root)

    def find_path(self, T):
        """find_path(T)
        Given a tree, find the path for a single data point based
        on the splitting criteria stored at each node.
        Parameters
        ----------
        T : iTree object (iTree.root object)

        Returns
        -------
        int
            The depth reached by the data point.
        """
        if T.ntype == 'LeafNode':

            if T.size <= 1:
                return self.e
            else:
                self.e = self.e + cy_usporf.c_factor(T.size)
                return self.e
        else:
            Aj = T.Aj
            splitPoint = T.splitPoint
            self.e += 1

            if np.dot(self.x, Aj) < splitPoint:
                self.path_list.append('L')
                return self.find_path(T.left)
            else:
                self.path_list.append('R')
                return self.find_path(T.right)


def all_branches(node, current=[], branches=None):
    """
    Utility function used in generating a graph visualization.
    It returns all the branches of a given tree so they can be visualized.

    Parameters
    ----------
    node: Node object

    Returns
    -------
    list
        list of branches that were reached.
    """
    current = current[:node.e]
    if branches is None:
        branches = []
    if node.ntype == 'inNode':
        current.append('L')
        all_branches(node.left, current=current, branches=branches)
        current = current[:-1]
        current.append('R')
        all_branches(node.right, current=current, branches=branches)
    else:
        branches.append(current)
    return branches
