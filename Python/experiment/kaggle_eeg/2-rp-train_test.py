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

import rerf
from rerf.rerfClassifier import rerfClassifier
print(rerf.__file__)

# In[2]:


## Paths
base_dir = Path('/mnt/ssd3/ronan/grasp-and-lift-eeg-detection')
load_dir = base_dir / 'processed'

# Columns name for labels
cols = ['HandStart','FirstDigitTouch',
        'BothStartLoadPhase','LiftOff',
        'Replace','BothReleased']

# Number of subjects
subjects = range(1,13)

# data path
prelag = 500
postlag = 500


# ## Helper functions

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

def load_data(data_dir, lags, subjects):
    """
    Loads processed X,y data
    """
    X = []
    y = []
    for subject in subjects:
        h5 = h5py.File(data_dir / f'subj{subject}_prelag={lags[0]}_postlag={lags[1]}_Xy.hdf5','r')
        X.append(h5['X'][:])
        y.append(h5['y'][:])
        h5.close()
    X = np.concatenate(X, axis=0)
    y = np.concatenate(y)

    return(X,y)


# ## Train & Test

# In[4]:


# Load subject(s) concatenated together
subjects = [1]
X,y = load_data(load_dir, [prelag, postlag], subjects)


# In[5]:


# Parameters
n_est = 500
ncores = 25
max_features = int(math.sqrt(X.shape[1])/4)
HEIGHT = 32
hmax = 6
hmin = 1
WIDTH = int(X.shape[1] / HEIGHT)
wmax = 10
wmin = 4


# In[6]:


# Define classifier(s)
names = {"TORF":"red"}

classifiers = [rerfClassifier(
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
                )]


# ### Optimize Parameters

# In[7]:


# Create write file
timestamp = datetime.now().strftime('%m-%d-%H:%M')
f = open(f'EEG_GridSearchCV_{timestamp}.csv', 'w+')
f.write("classifier,parameters,mean_test_score,mean_fit_time,mean_score_time\n")
f.flush()


# In[ ]:


# Grid search each classifier and write to file
parameters = [{'patch_height_max':np.arange(4,13,2), 'patch_height_min':[1],
               'patch_width_max':[50,100,200,400], 'patch_width_min':[25]}]
best_classifiers = []

for clf,name,parameters in tqdm(zip(classifiers, names, parameters)):
    gscv = GridSearchCV(clf, parameters, cv=StratifiedShuffleSplit(n_splits=3, test_size=0.1) ,n_jobs=ncores, refit=False)
    gscv.fit(X, y)

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

    #best_classifiers.append(gscv.best_estimator_)


# ### Accuracy vs. number of training samples

# In[25]:


# # cross-validation train & test splits preserving class percentages
# k = 2
# test_fraction = 0.1
# sss = StratifiedShuffleSplit(n_splits=k, test_size=test_fraction, random_state=0)

# # Number of training samples
# ns = np.linspace(2,np.log10(math.floor(len(y)*(1-test_fraction))),5)
# ns = np.power(10,ns).astype(int)


# In[ ]:


# # Train & Test
# timestamp = '{%m-%d-%H:%M:%S}'.format(datetime.datetime.now())
# f = open(f'EEG_results_{timestamp}.csv', 'w+')
# f.write("classifier,n,Lhat,trainTime,testTime,iterate\n")
# f.flush()

# runList = [(n, clf, name) for n in ns\
#            for clf,name in zip(classifiers, [name for name in names])]


# for i, (train_index, test_index) in enumerate(sss.split(X, y)):
#     print(f'Fold {i}')
#     bal_index = sort_keep_balance(y[train_idx],ns)
    
#     for n, clf, name in tqdm(runList)
    
    


# In[ ]:


# clf = rerfClassifier(n_estimators = n_est, projection_matrix = "RerF",
#             max_features = 28, n_jobs = ncores)

# logpath = Path('/home/rflperry/mf_sims')

# logging.basicConfig(filename=logpath / 'fashionmnist_mf_logging.log',
#                         format='%(asctime)s:%(levelname)s:%(message)s',
#                         level=logging.DEBUG
#                         )
# logging.info('NEW MF FashionMnist RUN')

# for n in ns:
#     logging.info(f'Test size: {n}')
#     clf.fit(X_train[0:n,::], y_train[0:n])
#     yhat_test = clf.predict(X_test)
#     logging.info(f'Accuracy {np.mean(y_test == yhat_test)}')

