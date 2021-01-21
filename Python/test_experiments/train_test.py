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
import pickle
import os
import re
import glob

import matplotlib.pyplot as plt
import seaborn as sns

from sklearn.model_selection import StratifiedShuffleSplit, GridSearchCV
from sklearn.preprocessing import StandardScaler
from data_loader import load_cifar10
from classifiers import Resnet50

import rerf
from rerf.rerfClassifier import rerfClassifier

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
    class_idxs = []
    for i in clss:
        idxs = np.where(y==i)[0]
        np.random.shuffle(idxs)
        class_idxs.append(idxs)

    sort_idxs = []
    
    prior_idxs = np.zeros(len(clss)).astype(int)
    for n in block_lengths:
        get_idxs = np.rint(n*ratios).astype(int)
        for idxs, prior_idx, next_idx in zip(class_idxs, prior_idxs, get_idxs):
            sort_idxs.append(idxs[prior_idx:next_idx])
        prior_idxs = get_idxs
        
    sort_idxs = np.hstack(sort_idxs)
    
    return(sort_idxs)


def load_data(dataset, model):
    if dataset == 'cifar10':
        X_train, y_train, X_test, y_test, data_shape = load_cifar10()

    return X_train, y_train, X_test, y_test, data_shape


def load_classifier(name, data_shape, n_classes=None):
    width, height, depth = data_shape
    n_est = 500
    ncores = 40
    max_features = 300
    hmin = 1
    hmax = 3
    wmin = 1
    wmax = 3
    if name == 'MORF':
        print("Rerf file (to verify dev version): " +rerf.__file__)
        clf = rerfClassifier(
            projection_matrix="S-RerF", 
            max_features=max_features,
            n_estimators=n_est,
            n_jobs=ncores,
            image_height=height, 
            image_width=width, 
            patch_height_max=hmax,
            patch_height_min=hmin,
            patch_width_max=wmax,
            patch_width_min=wmin
        )
    elif name == 'RF':
        clf = rerfClassifier(
            projection_matrix="Base", 
            max_features=max_features,
            n_estimators=n_est,
            n_jobs=ncores,
        )
    elif name == 'ResNet50':
        clf = Resnet50(data_shape, n_classes)
    else:
        raise ValueError(f"Classifier {name} not recongized")

    return clf


def fit_model(model, dataset, n_runs=3):
    X_train, y_train, X_test, y_test, data_shape = load_data(dataset, model)
    clf = load_classifier(model, data_shape, len(set(y_train)))
    ### Accuracy vs. number of training samples

    # Number of training samples
    ns = np.linspace(2,math.ceil(np.log10(len(y_train))),5)
    ns = np.power(10,ns).astype(int)

    # Train & Test
    timestamp = datetime.now().strftime('%m-%d-%H:%M')
    dir_to_save = f'./{dataset}/results/'
    if not os.path.exists(dir_to_save):
        os.makedirs(dir_to_save)

    for i in range(n_runs):
        files = glob.glob(dir_to_save + f"{model}*.pkl")
        max_run = max(int(re.findall(f".*/{model}.*_(\d+)\.pkl", f)[0]) for f in files) if files else 0
        run = max_run + 1
        print(f'Run {run} ({i+1}/{n_runs})')
        bal_index = sort_keep_balance(y_train, ns, random_state=run)
        for n in tqdm(ns):
            
            results_dict = {}
            # Preprocess
            n_index = bal_index[:n]
            ss = StandardScaler()
            Xn_train = ss.fit_transform(X_train[n_index])

            # Train
            trainStartTime = time.time()
            clf.fit(Xn_train, y_train[n_index])
            trainEndTime = time.time()
            train_time = trainEndTime - trainStartTime
            del Xn_train

            # Test
            Xn_test = ss.transform(X_test)
            testStartTime = time.time()
            # yhat = clf.predict(Xn_test)
            probs = clf.predict_proba(Xn_test)
            testEndTime = time.time()
            test_time = testEndTime - testStartTime
            del Xn_test

            results_dict['n'] = n
            results_dict['y'] = y_test
            results_dict['model'] = model
            results_dict['y_hat'] = np.argmax(probs, axis=1)
            results_dict['y_probs'] = probs
            results_dict['train_time'] = train_time
            results_dict['test_time'] = test_time
            accuracy = np.mean(y_test == results_dict['y_hat'])
            print(f'Accuracy: {accuracy}')
            # Save
            file_to_save = dir_to_save + f'{model}_{n}_{run}.pkl'
            with open(file_to_save, 'wb') as f:
                pickle.dump(results_dict, f)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--model", help="classifier to use", type=str, required=True, choices=['ResNet50', 'MORF', 'RF'])
    parser.add_argument("--dataset", help="data or experiment to run on", type=str, required=True, choices=['cifar10'])
    parser.add_argument("--nruns", help="The number of times to train/test", type=int)

    args = parser.parse_args()

    fit_model(args.model, args.dataset, args.nruns)

