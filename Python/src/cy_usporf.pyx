import random as rn
import cython
import numpy as np

from libc.stdlib cimport rand, RAND_MAX
from libc.math cimport log as ln  # base e
from libc.math cimport M_PI
from sklearn.tree._utils cimport log  # base 2
cimport numpy as np

np.import_array()

DTYPE = np.int
ctypedef np.int_t DTYPE_t
FTYPE = np.float
ctypedef np.float_t FTYPE_t
cdef double INFINITY = np.inf
DEF CUTOFF = 17
cdef double Euler_const = 0.5772156649


def c_factor(int n):
    """c_factor(int n)
    Average path length of unsuccesful search in a binary
    search tree given n points, using `ln` in C

    Parameters
    ----------
    n : int
        Number of data points for the binary search tree
        (BST)

    Returns
    -------
    float
        Average path length of unsuccesful search in a BST
    """
    return 2.0*(ln(n-1)+Euler_const) - (2.0*(n-1.)/(n*1.0))


def projectA(int dim, int d, double Lambda=0.05):
    """Sparse projected matrix
    Create a sparse matrix `A` with shape(dim, d)
    Lambda = non-zero element (-1 and 1)
    Note: non-zero element must be at least one

    Parameters
    ----------
    dim : int
        Number of data raw features
    d : int
        Number of data projected features

    Returns
    -------
    A : ndarray of shape (dim, d)
        projected matrix
    """
    cdef int i, i_new
    cdef non_zeros = 0
    cdef int A_size = dim * d
    cdef double p  # random probability
    cdef double Lambda_2 = Lambda*0.5
    cdef A = np.zeros((A_size,), dtype=float)

    for i in range(A_size):
        p = uniform()
        if p < Lambda_2:
            A[i] = 1
            non_zeros += 1
        elif (p >= Lambda_2) and (p < Lambda):
            A[i] = -1
            non_zeros += 1
    if non_zeros == 0:  # there is no non-zero
        i_new = int(rand()/RAND_MAX*A_size)
        if (rand()/RAND_MAX) < 0.5:
            A[i_new] = 1
        else:
            A[i_new] = -1
    return A.reshape(dim, d)


cdef double uniform():
    """
    random number uniformly from [0,1]
    """
    cdef double x1 = rand()
    return x1/RAND_MAX


@cython.boundscheck(False)  # Deactivate bounds checking
@cython.wraparound(False)   # Deactivate negative indexing.
def FastBIC(np.ndarray[FTYPE_t, ndim=1] X_1d):
    """Fast Bayesian Information Criterion score
    Calculate the splitPoint and
    min Bayesian information criterion (BIC) score
    from 1D float array

    Parameters
    ----------
    X_1d : ndarray of shape (N,)
        projected matrix

    Returns
    -------
    split_Point :  float
        splitting position
    minBIC :  float
        minimum BIC score
    """
    cdef int N = X_1d.shape[0]
    cdef np.ndarray[FTYPE_t, ndim=1] X_sorted = sort(X_1d)
    cdef np.ndarray[FTYPE_t, ndim=1] X_sorted_1
    cdef np.ndarray[FTYPE_t, ndim=1] X_sorted_2
    cdef FTYPE_t split_Point = X_sorted[0]
    cdef double minBIC = INFINITY
    cdef double BIC_diff_var, BIC_same_var, var_comb

    cdef Py_ssize_t s
    cdef int n_1, n_2
    cdef double pi_0 = 1.0
    cdef double pi_1, pi_2

    # var = sum(X-mu)^2/N = X_sumsq/N - (X_sum/N)^2
    cdef double X_sum = np.sum(X_sorted)
    cdef double X_sumsq = np.sum(X_sorted**2)
    cdef double X_sum_1 = 0
    cdef double X_sumsq_1 = 0
    cdef double X_sum_2 = X_sum
    cdef double X_sumsq_2 = X_sumsq
    cdef double i_1 = 1
    cdef double i_2 = N-1
    cdef double var_1 = 0.0
    cdef double var_2 = 0.0

    pi_0 = pi_0/N
    for s in range(0, N-1):
        n_1 = s
        n_2 = N-s

        X_sum_1 += X_sorted[s]
        X_sumsq_1 += X_sorted[s]**2
        X_sum_2 -= X_sorted[s]
        X_sumsq_2 -= X_sorted[s]**2

        var_1 = (X_sumsq_1)/(n_1+1) - (X_sum_1/(n_1+1))**2
        if (var_1 == 0):
            continue
        var_2 = (X_sumsq_2)/(n_2-1) - (X_sum_2/(n_2-1))**2
        if (var_2 == 0):
            continue

        pi_1 = s*pi_0
        pi_2 = 1-pi_1
        var_comb = (pi_1*var_1 + pi_2*var_2)
        BIC_diff_var = -2*(n_1 * ln(pi_1)
                           - n_1/2 * ln(2*M_PI*var_1)
                           + n_2 * ln(pi_2)
                           - n_2/2 * ln(2*M_PI*var_2))
        BIC_same_var = -2*(n_1 * ln(pi_1)
                           - n_1/2 * ln(2*M_PI*var_comb)
                           + n_2 * ln(pi_2)
                           - n_2/2 * ln(2*M_PI*var_comb))

        if BIC_diff_var > BIC_same_var:
            BIC_diff_var = BIC_same_var

        if BIC_diff_var < minBIC:
            minBIC = BIC_diff_var
            split_Point = (X_sorted[s-1] + X_sorted[s])/2
    return(split_Point, minBIC)


cdef np.ndarray[FTYPE_t, ndim=1] sort(np.ndarray[FTYPE_t, ndim=1] a):
    """Fast Sorting Function
    Parameters
    ----------
    a : ndarray of shape (N,)
        projected matrix

    Returns
    -------
    ndarray of shape (N,)
        projected matrix
    """
    qsort(<FTYPE_t*> a.data, 0, a.shape[0])
    return a

cdef void qsort(FTYPE_t * a, Py_ssize_t start, Py_ssize_t end):
    if (end - start) < CUTOFF:
        insertion_sort(a, start, end)
        return
    cdef Py_ssize_t boundary = partition(a, start, end)
    qsort(a, start, boundary)
    qsort(a, boundary+1, end)

cdef Py_ssize_t partition(FTYPE_t * a, Py_ssize_t start, Py_ssize_t end):
    assert end > start
    cdef Py_ssize_t i = start, j = end-1
    cdef FTYPE_t pivot = a[j]
    while True:
        # assert all(x < pivot for x in a[start:i])
        # assert all(x >= pivot for x in a[j:end])

        while a[i] < pivot:
            i += 1
        while i < j and pivot <= a[j]:
            j -= 1
        if i >= j:
            break
        assert a[j] < pivot <= a[i]
        swap(a, i, j)
        assert a[i] < pivot <= a[j]
    assert i >= j and i < end
    swap(a, i, end-1)
    assert a[i] == pivot
    # assert all(x < pivot for x in a[start:i])
    # assert all(x >= pivot for x in a[i:end])
    return i

cdef inline void swap(FTYPE_t * a, Py_ssize_t i, Py_ssize_t j):
    a[i], a[j] = a[j], a[i]

cdef void insertion_sort(FTYPE_t * a, Py_ssize_t start, Py_ssize_t end):
    cdef Py_ssize_t i, j
    cdef FTYPE_t v
    for i in range(start, end):
        # invariant: [start:i) is sorted
        v = a[i]
        j = i-1
        while j >= start:
            if a[j] <= v:
                break
            a[j+1] = a[j]
            j -= 1
        a[j+1] = v
