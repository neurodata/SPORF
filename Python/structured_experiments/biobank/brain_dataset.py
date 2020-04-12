#!/usr/bin/env python
# coding: utf-8

# In[52]:


import gc
import math
import pandas as pd
import numpy as np
import time, multiprocessing
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from tqdm import tqdm
import seaborn as sns
from pathlib import Path

import pickle
import logging

from rerf.rerfClassifier import rerfClassifier

from scipy import stats
from sklearn.model_selection import StratifiedShuffleSplit
from sklearn.model_selection import StratifiedKFold
from sklearn.model_selection import cross_validate
from sklearn.model_selection import GridSearchCV
from sklearn.metrics import balanced_accuracy_score

from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.linear_model import LogisticRegression

gc.enable()

import warnings
warnings.simplefilter('ignore')


# In[2]:


# In[53]:

def run(X,y,idx, file, skip_opti=True):
    img_height = X.shape[1]
    img_width = X.shape[2]

    X = X.reshape(X.shape[0], -1)
    n_features = X.shape[1]

    # In[58]:

    ncores=1
    num_runs=1
    n_est=500
    mx = 10
    mn = 1

    ## sqrt(n_features) = 99
    feature_range = [10,30,60,100,150]
    patch_max = [5,8,11,15,20]

    names = {"RF":"#fb9a99", 
            "SPORF":"#ff7f00", 
            "MORF":"#e31a1c"}

    rf_max_features = 100 #'auto
    sporf_max_features = 100 #'auto
    morf_max_features = 100 #'auto

    clfs = [
        RandomForestClassifier(n_estimators=n_est, max_features=rf_max_features, n_jobs=ncores),
        rerfClassifier(n_estimators = n_est, projection_matrix = "RerF",
                max_features = sporf_max_features, n_jobs = ncores),
        rerfClassifier(
                projection_matrix="S-RerF", 
                max_features = morf_max_features,
                n_estimators=n_est,
                n_jobs=ncores,
                image_height=img_height, 
                image_width=img_width, 
                patch_height_max=6,
                patch_width_max=8,
                patch_height_min=1,
                patch_width_min=1,
            )]


    # In[55]:


    logdir = Path('./')

    logging.basicConfig(filename=logdir / 'biobank_logging.log',
                            format='%(asctime)s:%(levelname)s:%(message)s',
                            level=logging.DEBUG
                            )
    logging.info(f'NEW MF Biobank Brain Data RUN: {file}')


    # In[56]:
    ns = idx.keys()

    # In[68]:


    parameters = [{'max_features':feature_range},
                {'max_features':feature_range},
                {'max_features':feature_range, 'patch_height_max': patch_max, 'patch_width_max':patch_max}]
    skf = StratifiedShuffleSplit(n_splits=3)

    best_clfs = []

    #clfs = [LogisticRegression(random_state=0, n_jobs=ncores, solver='sag',
    #        penalty='l2',C = 1.0, class_weight='balanced', max_iter=1000)]
    #parameters = [{'C':np.linspace(0.1,2,11)}]
    #names = {'LinReg':''}
    if skip_opti == True:
        logging.info(f'Using fixed params')
        best_clfs = clfs
    else:
        for name,params,clf in zip(names.keys(),parameters,clfs):
            idx_train,_,_ = idx[max(ns)][0]
            logging.info(f'Grid Search Classifier {name}')
            gs = GridSearchCV(clf, params, cv=skf, verbose=1)
            gs.fit(X[idx_train,:], y[idx_train])
            best_clfs.append(gs.best_estimator_)
            logging.info(f'Best Parameters: {str(gs.best_estimator_)}')


    # In[67]:

    ## Prep output file:

    write_path = f'biobank_{file}.csv'

    with open(write_path, 'w') as f:
        f.write("classifier,n,Lhat,trainTime,testTime,iterate\n")

    runList = [(n, clf, iterate) for n in ns                             for clf in zip(best_clfs, [key for key in names])
                                for iterate in range(1, num_runs + 1)]

    for n, clf, iterate in tqdm(runList):
        logging.info(f'Run:n={n},clf={clf[1]}')
        gc.collect()

        train_idx, val_idx, test_idx = idx[max(idx.keys())][0]
        
        y_train = y[train_idx]
        X_train = X[train_idx,:]

        y_test = y[test_idx]
        X_test = X[test_idx,:]

        trainStartTime = time.time()
        clf[0].fit(X_train, y_train)
        trainEndTime = time.time()
        trainTime = trainEndTime - trainStartTime

        testStartTime = time.time()
        out = clf[0].predict(X_test)
        testEndTime = time.time()
        testTime = testEndTime - testStartTime

        lhat = np.mean(np.not_equal(out, y_test).astype(int))

        ####("variable,Lhat,trainTime,testTime,iterate")
        with open(write_path, 'a') as f:
            f.write(f"{clf[1]}, {n}, {lhat:2.9f}, {trainTime:2.9f}, {testTime:2.9f}, {iterate}\n")

        gc.collect()

DATA_PATH = Path('/mnt/c/Users/Ronan Perry/Documents/JHU/jovo-lab/rerf/morf_real_data/')
files = ['slice_axial_smri_ageXsex.p',
         'slice_coronal_smri_ageXsex.p',
         'slice_sagittal_smri_ageXsex.p']

if __name__ == '__main__':
    for file in files:
        with open(DATA_PATH / file, 'rb') as f:
                X,y,idx = pickle.load(f)
        run(X,y,idx,file)



