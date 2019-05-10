from RerF import fastRerF, fastPredict, retSimMat
import numpy as np
import time, os, graspy
from graspy.embed import *
from multiprocessing import cpu_count
from scipy.sparse import *
from scipy.sparse.linalg import svds, eigs
import scipy.linalg as la
import pandas as pd
import matplotlib.pyplot as plt

sim_mats = []
endtime = 40000 - 501
cnt=0
filepath = "/mnt/nfs2/dmargulies/data/mpi/timeseries"
files = os.listdir(filepath)
ntree = 500
#ntree = 500
for ts_file in files:
    ts_fpath = filepath + "/" + ts_file
    fname = ts_file[:-4]
    print(fname)
    ts_vec = np.load(ts_fpath)
    
    Y = np.ones(ts_vec.shape[0])
    Y[0] = 0
    
    ts_vec[np.where(ts_vec < 0.0001)] = 0
    forest = fastRerF(
    X=ts_vec,
    Y=Y,
    forestType="urf",
    trees=ntree,
    minParent=100, 
    numCores=cpu_count() - 2,
    )
    pairMat = retSimMat(forest)

    tupList = []
    dataCounts = []

    for key, value in pairMat.items():
        tupList.append(key)
        dataCounts.append(value/ntree)
        tupList.append((key[1], key[0]))
        dataCounts.append(value/ntree)

    row = []
    col = []

    for tup in tupList:
        row.append(tup[0])
        col.append(tup[1])

    X_new = ts_vec
    sparse_mat = csr_matrix((dataCounts, (row, col)) , shape=(X_new.shape[0], X_new.shape[0]))
    sparse_mat = sparse_mat.astype(float)
    np.savez( fname, data=sparse_mat.data, indices = sparse_mat.indices, indptr = sparse_mat.indptr, shape=sparse_mat.shape)

