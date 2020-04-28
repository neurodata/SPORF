
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

DATASET = 'UWaveGestureLibraryX'
X_train,y_train,X_test,y_test = load_data(DATASET)
# print(np.unique(y_train))
# for i in np.unique(y_train):
#     print(f'{i}: {sum(y_train==i)}')
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

# Grid search each classifier and write to file
parameters = [{'patch_height_max':[1], 'patch_height_min':[1],
               'patch_width_max':[2,3,4,5,6,8,10,12,20], 'patch_width_min':[1,2],
               'max_features':[5,10,25,50,100]}]
best_classifiers = []

timestamp = datetime.now().strftime('%m-%d-%H:%M')
f = open(f'gscv_results_{DATASET}_{timestamp}.csv', 'w+')
f.write("classifier,parameters,mean_test_score,mean_fit_time,mean_score_time\n")
f.flush()

for clf,name,parameters in tqdm(zip(classifiers, names, parameters)):
    gscv = GridSearchCV(clf, parameters, cv=StratifiedShuffleSplit(n_splits=1,
        test_size=0.1),n_jobs=ncores, refit=False, scoring='balanced_accuracy')
    gscv.fit(X_train, y_train)

    results = gscv.cv_results_

    result_keys = ['params','mean_test_score',
                  'mean_fit_time','mean_score_time']

    cv_results = [', '.join([str(results[key][i]) for key in result_keys]) for i in range(len(results['params']))]
    for cv_result in cv_results:
        f.write(f'{name}, {cv_result}\n')
        f.flush()

    print(f'Best {name} parameters:')
    print(f'{gscv.best_params_}')
    print(f'With score {gscv.best_score_}')