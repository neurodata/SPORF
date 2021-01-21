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
from sklearn.metrics import balanced_accuracy_score

import rerf
from rerf.rerfClassifier import rerfClassifier
from data_loader import load_data
print(rerf.__file__)

# ## Helper functions

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

# ## Train & Test

# In[4]:

DATASET = 'Phoneme'
X_train,y_train,X_test,y_test = load_data(DATASET)
# In[5]:


# Parameters
n_est = 500
ncores = 40
max_features = 500
HEIGHT = 1
hmin = 1
hmax = 1
WIDTH = int(X_train.shape[1] / HEIGHT)
wmax = 4
wmin = 1


# In[6]:


# Define classifier(s)
# names = {"RF":"#f86000", "TORF":"red", "MORF": "orange"}
names = {"TORF":"red"}
classifiers = [RandomForestClassifier(n_estimators=n_est, max_features='auto', n_jobs=ncores),
                rerfClassifier(
                    projection_matrix="MT-MORF", 
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

# cross-validation train & test splits preserving class percentages
# k = 1
# test_fraction = 0.1
# sss = StratifiedShuffleSplit(n_splits=k, test_size=test_fraction, random_state=0)

# Number of training samples
ns = np.linspace(2,np.log10(len(y_train)),5)
ns = np.power(10,ns).astype(int)

# Train & Test
timestamp = datetime.now().strftime('%m-%d-%H:%M')
f = open(f'classifier_results_{DATASET}_{timestamp}.csv', 'w+')
f.write("classifier,n,balancedError,trainTime,testTime,iterate\n")
f.flush()

clf = classifiers[0]
name = 'TORF'
i = 0
bal_index = sort_keep_balance(y_train,ns)
for n in ns:
    nIndex = bal_index[:n]

    trainStartTime = time.time()
    clf.fit(X_train[nIndex], y_train[nIndex])
    trainEndTime = time.time()
    trainTime = trainEndTime - trainStartTime

    testStartTime = time.time()
    yhat = clf.predict(X_test)
    testEndTime = time.time()
    testTime = testEndTime - testStartTime

    #predictions.append([i, n] + list(yhat))

    bal_err = 1 - balanced_accuracy_score(y_test, yhat)
    #lhat = np.mean(np.not_equal(yhat, y[test_index]).astype(int))

    ####("variable,Lhat,trainTime,testTime,iterate")
    f.write(f"{name}, {n}, {bal_err:2.9f}, {trainTime:2.9f}, {testTime:2.9f}, {i}\n")
    f.flush()

f.close()

#np.savetxt(f'Predictions_{TAG}_{timestamp}.csv', predictions, delimiter=',')