# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %% [markdown]
# # Structured RerF Demo: Impulse Response
# We form a two-class classification problem where each data point is 100 timesteps. Class 0 is simply white noise while class 1 is white noise plus an exponentially decaying unit impulse beginning at timestep 20.
#
# We test the performance of S-Rerf and a set of other classification algorithms, training each on samples of sizes $n \in \{50,100,200,400,1000,2000\}$, each containing an equal number of data points in each class. The average 0-1 loss is evaluated for each algorithm for each training size using a single test set of size $m=10000$ with an equal number of data points in each class.

# %%
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
from datetime import datetime
import pickle
from glob import glob

import rerf
from rerf.rerfClassifier import rerfClassifier
from graspy.plot import heatmap

from scipy.stats import bernoulli
from scipy import stats

from sklearn.gaussian_process.kernels import RBF
from sklearn.tree import DecisionTreeClassifier
from sklearn.naive_bayes import GaussianNB
from sklearn.discriminant_analysis import QuadraticDiscriminantAnalysis

from sklearn.linear_model import LogisticRegression
from sklearn.svm import LinearSVC
from sklearn.svm import SVC
from sklearn.ensemble import RandomForestClassifier, AdaBoostClassifier
from sklearn.neural_network import MLPClassifier
from sklearn.neighbors import KNeighborsClassifier
from sklearn.model_selection import GridSearchCV

import sys

sys.path.append("..")
from utils.load_Xy import *
from utils.utils import plot_data
from base_classifiers import get_classifiers

gc.enable()

import warnings

warnings.simplefilter("ignore")


# %%
ns = np.array([10, 50, 100, 200, 400, 1000, 2000])
n0 = int(max(ns) / 2)
n1 = n0
n_test = 10000

X_train, y_train, _ = load_impulse_Xy(n0, n1, ns, seed=0)
X_test, y_test, size_dict = load_impulse_Xy(
    int(n_test / 2), int(n_test / 2), ns, seed=1
)


# %%
## Setup for run
names = {
    "Log. Reg": "#a6cee3",
    "Lin. SVM": "#1f78b4",
    "SVM": "#b2df8a",
    "kNN": "#33a02c",
    "RF": "#fb9a99",
    "MLP": "#fdbf6f",
    "SPORF": "#ff7f00",
    "MF": "#e31a1c",
    "CNN": "#cab2d6",
}

ncores = 40
n_est = 500

classifiers = [
    LogisticRegression(random_state=0, n_jobs=ncores, solver="liblinear"),
    LinearSVC(),
    SVC(C=1.0, kernel="rbf", gamma="auto", random_state=0),
    KNeighborsClassifier(3, n_jobs=ncores),
    RandomForestClassifier(n_estimators=n_est, max_features="auto", n_jobs=ncores),
    MLPClassifier(
        hidden_layer_sizes=(100,), random_state=0, max_iter=1000, early_stopping=True
    ),
    rerfClassifier(
        n_estimators=n_est,
        projection_matrix="RerF",
        max_features="auto",
        n_jobs=ncores,
        random_state=0,
    ),
    rerfClassifier(
        projection_matrix="S-RerF",
        max_features="auto",
        n_jobs=ncores,
        n_estimators=n_est,
        oob_score=False,
        random_state=0,
        image_height=size_dict["height"],
        image_width=size_dict["width"],
        patch_height_max=1,
        patch_height_min=1,
        patch_width_max=2,
        patch_width_min=1,
    ),
]
## GSCV params
metric = "accuracy"
cv = 3  # number of splits stratifiedKFold
param_grid = [
    {"C": [0.01, 0.1, 1, 10, 100]},
    {"C": [0.01, 0.1, 1, 10, 100]},
    [
        {
            "C": [0.1, 1, 10, 100],
            "gamma": [0.01, 0.001, 0.0001],
            "kernel": ["poly"],
            "degree": [2, 3],
            "coef0": [0, 0.1, 1],
        },
        {"C": [0.1, 1, 10, 100], "gamma": [0.01, 0.001, 0.0001], "kernel": ["rbf"]},
        {
            "C": [0.1, 1, 10, 100],
            "gamma": [0.01, 0.001, 0.0001],
            "kernel": ["sigmoid"],
            "coef0": [0, 0.1, 1],
        },
    ],
    {"n_neighbors": [3, 4, 5, 6, 7], "p": [1, 2, 3]},
    {"max_features": [0.1, 0.3, 0.5, 0.7]},
    {
        "hidden_layer_sizes": [(100,), (100, 100), (50, 100, 50)],
        "activation": ["tanh", "relu"],
        "alpha": [0.0001, 0.001, 0.01, 0.1],
        "learning_rate": ["constant", "adaptive"],
    },
    {"max_features": [0.1, 0.3, 0.5, 0.7]},
    {
        "max_features": [0.1, 0.3, 0.5, 0.7],
        "patch_height_max": [1],#[2,3,4,5],
        "patch_height_min": [1],
        "patch_width_max": np.arange(2,13,2),#[2,3,4,5],
        "patch_width_min": [1],
    },
]


# %%
# Train each classifier on each dataset size, then test
## Prep output file:
save_dir = Path("./optimization_results")
timestamp = datetime.now().strftime('%m-%d-%H:%M')

for params,clf,name in tqdm(zip(param_grid,classifiers,names)):
    if len(glob(str(save_dir / f'{name}_*'))) > 1:
        continue
    gscv = GridSearchCV(clf, params, cv=cv, scoring=metric,n_jobs=ncores, refit=False)
    gscv.fit(X_train, y_train)

    results = gscv.cv_results_

    result_keys = ['params','mean_test_score',
                  'mean_fit_time','mean_score_time']

    cv_results = [', '.join([str(results[key][i]) for key in result_keys]) for i in range(len(results['params']))]
    
    f = open(save_dir / f'{name}_{timestamp}_gscv_results.csv', 'w+')
    f.write("classifier,parameters,mean_test_score,mean_fit_time,mean_score_time\n")
    f.flush()
    for cv_result in cv_results:
        f.write(f'{name}, {cv_result}\n')
        f.flush()
    f.close()

    best_params = {'name':name,'params': gscv.best_params_, 'score':gscv.best_score_}
    pickle.dump(best_params, open(save_dir / f'{name}_{timestamp}_best_params.pkl', "wb"))