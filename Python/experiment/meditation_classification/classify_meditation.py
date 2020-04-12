#!/usr/bin/env python
# coding: utf-8

# # Evaluate Timeseries Rerf Performance on meditation data

# In[1]:

from utils import load_Xy
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

from sklearn.model_selection import train_test_split
from sklearn.linear_model import LogisticRegression
from sklearn.svm import LinearSVC
from sklearn.svm import SVC
from sklearn.ensemble import RandomForestClassifier
from sklearn.neural_network import MLPClassifier
from sklearn.neighbors import KNeighborsClassifier

import rerf
from rerf.rerfClassifier import rerfClassifier
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

# ## Train & Test

# In[4]:


# Load subject(s) concatenated together
X,y = load_Xy()
y = (y/3).astype(int)
print(len(set(y)))

# In[5]:


# Parameters
n_est = 500
ncores = 10
max_features = 50#int(math.sqrt(X.shape[1])/2)
HEIGHT = 18715
hmin = 10
hmax = 200
WIDTH = int(X.shape[1] / HEIGHT)
wmax = 25
wmin = 5


# In[6]:


# Define classifier(s)
# "Log. Reg": "blue",
names = {"RF":"#f86000", "TORF":"red", "MORF": "orange"}
#names = {"TORF":"red", "Lin. SVM":"firebrick",}
#LinearSVC(),
                #LogisticRegression(random_state=0, n_jobs=ncores, solver='liblinear'),
classifiers = [ RandomForestClassifier(n_estimators=n_est, max_features='auto', n_jobs=ncores),
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
                ),
                rerfClassifier(
                    projection_matrix="S-RerF", 
                    max_features=max_features,
                    n_estimators=n_est,
                    n_jobs=ncores,
                    image_height=HEIGHT, 
                    image_width=WIDTH, 
                    patch_height_max=1,
                    patch_height_min=1,
                    patch_width_max=wmax,
                    patch_width_min=wmin
                )
            ]

#classifiers = [classifiers[0], classifiers[2]]

### Accuracy vs. number of training samples

# cross-validation train & test splits preserving class percentages
k = 10
test_fraction = 0.1
sss = StratifiedShuffleSplit(n_splits=k, test_size=test_fraction, random_state=0)

# Number of training samples
# ns = np.linspace(2,np.log10(math.floor(len(y)*(1-test_fraction))),5)
# ns = np.power(10,ns).astype(int)
ns = [18,50,100,-1]

# Train & Test
# timestamp = datetime.now().strftime('%m-%d-%H:%M')
timestamp = datetime.now().strftime('%m-%d-%H:%M')
f = open(f'classification_results_meditation_{timestamp}.csv', 'w+')
f.write("classifier,n,Lhat,trainTime,testTime,iterate\n")
f.flush()
g = open(f'raw_predictions_meditation_{timestamp}.csv', 'w+')
g.flush()

runList = [(n, clf, name) for n in ns\
           for clf,name in zip(classifiers, [name for name in names])]


for i, (train_index, test_index) in enumerate(sss.split(X, y)):
    print(f'Fold {i}')
    #bal_index = sort_keep_balance(y[train_index],ns)

    g.write(f"{'Truth'}, {'-1'}, {i}, {','.join(map(str, list(y[test_index]))) }\n")
    g.flush()
    
    for n, clf, name in tqdm(runList):
        if n == -1:
            n = len(train_index)
        nIndex = train_index[:n]
        
        trainStartTime = time.time()
        clf.fit(X[nIndex], y[nIndex])
        trainEndTime = time.time()
        trainTime = trainEndTime - trainStartTime

        testStartTime = time.time()
        yhat = clf.predict(X[test_index])
        testEndTime = time.time()
        testTime = testEndTime - testStartTime

        g.write(f"{name}, {n}, {i}, {','.join(map(str, list(yhat))) }\n")
        g.flush()

        lhat = np.mean(np.not_equal(yhat, y[test_index]).astype(int))

        #### ("variable,Lhat,trainTime,testTime,iterate")
        f.write(f"{name}, {n}, {lhat:2.9f}, {trainTime:2.9f}, {testTime:2.9f}, {i}\n")
        f.flush()

f.close()
g.close()