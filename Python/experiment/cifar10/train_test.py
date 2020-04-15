#!/usr/bin/env python
# coding: utf-8

# # Evaluate Timeseries Rerf Performance

# In[1]:


import numpy as np
from pathlib import Path
import h5py
import math
import time
from datetime import datetime
from tqdm import tqdm

import matplotlib.pyplot as plt
import seaborn as sns

from sklearn.model_selection import StratifiedShuffleSplit
from sklearn.model_selection import GridSearchCV
from sklearn.ensemble import RandomForestClassifier

import rerf
from rerf.rerfClassifier import rerfClassifier
from data_loader import get_train, get_test
print(rerf.__file__)


# In[3]:


def sort_keep_balance(y,block_lengths):
    """
    Sort data and labels into blocks that preserve class balance
    
    Parameters
    ----------
    X: data matrix
    y : 1D class labels
    block_lengths : Block sizes to sort X,y into that preserve class balance
    """
    clss,counts = np.unique(y, return_counts=True)
    ratios = counts / sum(counts)
    class_idxs = [np.where(y==i)[0] for i in clss]

    sort_idxs = []
    
    prior_idxs = np.zeros(len(clss)).astype(int)
    for n in block_lengths:
        get_idxs = np.rint(n*ratios).astype(int)
        for idxs,prior_idx,next_idx in zip(class_idxs,prior_idxs,get_idxs):
            sort_idxs.append(idxs[prior_idx:next_idx])
        prior_idxs = get_idxs
        
    sort_idxs = np.hstack(sort_idxs)
    
    return(sort_idxs)

X_train,y_train = get_train()
y_train[np.where(y_train != 1)] = 0
X_test,y_test = get_test()
y_test[np.where(y_test != 1)] = 0

# Parameters
n_est = 1000
ncores = 40
max_features = 400
HEIGHT = 32
hmin = 1
hmax = 3
WIDTH = 32
wmin = 1
wmax = 3


# In[6]:


# Define classifier(s)
# names = {"RF":"#f86000", "TORF":"red", "MORF": "orange"}
names = {"TORF":"red"}
classifiers = [RandomForestClassifier(n_estimators=n_est, max_features='auto', n_jobs=ncores),
                rerfClassifier(
                    projection_matrix="S-RerF", 
                    max_features=max_features,
                    n_estimators=n_est,
                    n_jobs=ncores,
                    image_height=HEIGHT, 
                    image_width=WIDTH, 
                    patch_height_max=hmax,
                    patch_height_min=hmin,
                    patch_width_max=wmax,
                    patch_width_min=wmin
                )
            ]

classifiers = [classifiers[1]]

### Accuracy vs. number of training samples

# Number of training samples
ns = np.linspace(2,math.ceil(np.log10(len(y_train))),5)
ns = np.power(10,ns).astype(int)

# Train & Test
timestamp = datetime.now().strftime('%m-%d-%H:%M')
f = open(f'cifar10_{timestamp}.csv', 'w+')
f.write("classifier,n,Lhat,trainTime,testTime\n")
f.flush()

runList = [(n, clf, name) for n in ns\
           for clf,name in zip(classifiers, [name for name in names])]

bal_index = sort_keep_balance(y_train,ns)
for n, clf, name in tqdm(runList):
    nIndex = bal_index[:n]
    
    trainStartTime = time.time()
    clf.fit(X_train[nIndex], y_train[nIndex])
    trainEndTime = time.time()
    trainTime = trainEndTime - trainStartTime

    testStartTime = time.time()
    yhat = clf.predict(X_test)
    testEndTime = time.time()
    testTime = testEndTime - testStartTime

    lhat = np.mean(np.not_equal(yhat, y_test).astype(int))

    ####("variable,Lhat,trainTime,testTime,iterate")
    f.write(f"{name}, {n}, {lhat:2.9f}, {trainTime:2.9f}, {testTime:2.9f}\n")
    f.flush()

f.close()