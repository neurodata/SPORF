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
import argparse

import matplotlib.pyplot as plt
import seaborn as sns

from sklearn.model_selection import StratifiedShuffleSplit, GridSearchCV
from sklearn.preprocessing import StandardScaler
from sklearn.ensemble import RandomForestClassifier

import rerf
from rerf.rerfClassifier import rerfClassifier
from data_loader import get_train, get_test
print("Rerf file (to verify dev location): " +rerf.__file__)


# In[3]:


def sort_keep_balance(y, block_lengths, random_state=None):
    """
    Sort data and labels into blocks that preserve class balance
    
    Parameters
    ----------
    X: data matrix
    y : 1D class labels
    block_lengths : Block sizes to sort X,y into that preserve class balance
    """
    np.random.seed(random_state)
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

X_train, y_train = get_train()
X_test, y_test = get_test()

# Parameters
n_est = 500
ncores = 40
max_features = 150
HEIGHT = 32
hmin = 1
hmax = 3
WIDTH = 32
wmin = 1
wmax = 3


# Define classifier(s)
names = [
    # 'RF',
    'MORF'
]
classifiers = [# RandomForestClassifier(n_estimators=n_est, max_features='auto', n_jobs=ncores),
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

### Accuracy vs. number of training samples

# Number of training samples
ns = np.linspace(2,math.ceil(np.log10(len(y_train))),5)
ns = np.power(10,ns).astype(int)

# Train & Test
timestamp = datetime.now().strftime('%m-%d-%H:%M')
# f = open(f'cifar10_{timestamp}.csv', 'w+')
# f.write("classifier,n,Lhat,trainTime,testTime\n")
# f.flush()

runList = [(n, clf, name) for n in ns\
           for clf,name in zip(classifiers, [name for name in names])]

bal_index = sort_keep_balance(y_train, ns, random_state=0)

for n, clf, name in tqdm(runList):
    n_index = bal_index[:n]
    ss = StandardScaler()
    X_n = ss.fit_transform(X_train[n_index])
    trainStartTime = time.time()
    clf.fit(X_train[n_ndex], y_train[n_index])
    trainEndTime = time.time()
    trainTime = trainEndTime - trainStartTime

    testStartTime = time.time()
    yhat = clf.predict(X_test)
    probs = clf.predict_proba(X_test)[:,1]
    testEndTime = time.time()
    testTime = testEndTime - testStartTime

    prob_mtx.append(probs)
    np.savetxt(f'./cifar10_1vs-all_{timestamp}.csv', prob_mtx, delimiter=',')
    #lhat = np.mean(np.not_equal(yhat, y_test).astype(int))

    ####("variable,Lhat,trainTime,testTime,iterate")
    #f.write(f"{name}, {n}, {lhat:2.9f}, {trainTime:2.9f}, {testTime:2.9f}\n")
    #f.flush()
np.savetxt(f'./cifar10_1vs-all_{timestamp}.csv', prob_mtx, delimiter=',')
#f.close()