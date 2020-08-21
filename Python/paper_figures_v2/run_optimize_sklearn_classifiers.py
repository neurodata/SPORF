import gc
import math
from collections import defaultdict
import pandas as pd
import numpy as np
import time, multiprocessing
import matplotlib
from glob import glob
import pickle
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from tqdm import tqdm
import seaborn as sns
from pathlib import Path
from graspy.plot import heatmap
from scipy import stats
from utils.load_Xy import *
from utils.utils import plot_data, sort_keep_balance
from utils.base_classifiers import get_classifiers
from optimize_classifiers import optimize
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.utils import check_random_state
import argparse
from utils.cnn import *

gc.enable()

import sys
import os
import warnings
import io
from contextlib import redirect_stdout

warnings.simplefilter("ignore")

# %%
def get_data(experiment, data_loader, ns, normalize=True, n_test=10000):
    n0 = int(max(ns) / 2)
    n1 = n0
    n_test = 10000

    X_train, y_train, _ = data_loader(n0, n1, ns, seed=0)
    X_test, y_test, size_dict = data_loader(
        int(n_test / 2), int(n_test / 2), ns, seed=1
    )

    if normalize:
        scaler = StandardScaler()
        X_train = scaler.fit_transform(X_train)
        X_test = scaler.transform(X_test)

    return (X_train, y_train, X_test, y_test, size_dict)


def get_mnist(ns=None, normalize=True):
    X, y, size_dict = load_mnist_Xy(ns)
    test_size = len(X) - max(ns)

    X_train, X_test, y_train, y_test = train_test_split(
        X, y, train_size=max(ns), test_size=test_size
    )
    if normalize:
        scaler = StandardScaler()
        X_train = scaler.fit_transform(X_train)
        X_test = scaler.transform(X_test)

    if ns is not None:
        idxs = sort_keep_balance(y_train, ns)
        X_train = X_train[idxs]
        y_train = y_train[idxs]

    return (X_train, y_train, X_test, y_test, size_dict)

def get_cifar10(ns=None, normalize=True):
    X_train, y_train, X_test, y_test, size_dict = load_cifar10()

    if normalize:
        scaler = StandardScaler()
        X_train = scaler.fit_transform(X_train)
        X_test = scaler.transform(X_test)

    # if ns is not None:
    #     idxs = sort_keep_balance(y_train, ns)
    #     X_train = X_train[idxs]
    #     y_train = y_train[idxs]

    return (X_train, y_train, X_test, y_test, size_dict)


def get_mf_params(experiment):
    wrap = 0
    if experiment == "impulse":
        patch_height_maxs = [1]
        patch_width_maxs = np.arange(2, 13, 2)
        patch_height_mins = [1]
        patch_width_mins = [2]
    elif experiment == "hvbar":
        patch_height_maxs = [2, 3, 4, 5, 7, 9]
        patch_width_maxs = [2, 3, 4, 5, 7, 9]
        patch_height_mins = [2]
        patch_width_mins = [2]
    elif experiment == "circle":
        patch_height_maxs = [1]
        patch_width_maxs = [5,6,8,10,12]
        patch_height_mins = [1]
        patch_width_mins = [1,3,4,5]
        wrap = 1
    elif experiment == "mnist":
        patch_height_maxs = [2, 3, 4, 5]
        patch_width_maxs = [2, 3, 4, 5]
        patch_height_mins = [2]
        patch_width_mins = [2]
    elif experiment == "cifar10":
        patch_height_maxs = [2, 3, 4, 5]
        patch_width_maxs = [2, 3, 4, 5]
        patch_height_mins = [2]
        patch_width_mins = [2]
    else:
        print("ERROR: Please specify experimental patch sizes")

    return (
        patch_height_maxs,
        patch_height_mins,
        patch_width_maxs,
        patch_width_mins,
        wrap,
    )

def get_cnn(experiment, size_dict):
    model_dict = {
        "impulse": [ImpulseModel, (1, size_dict["width"])],
        "hvbar": [HVBarModel, (1, size_dict["height"], size_dict["width"])],
        "circle": [CircleModel, (1, size_dict["width"])],
        "mnist": [MNISTModel, (1, size_dict["height"], size_dict["width"])],
        "cifar10": [CifarModel, (3, size_dict["height"], size_dict["width"])],
    }
    ## Setup for run
    model, shape = model_dict[experiment]
    # Hyperparameters
    num_epochs = 1000
    learning_rate = 0.001
    batch_size=250

    cnn = CNN(
        model=model,
        shape=shape,
        num_epochs=num_epochs,
        learning_rate=learning_rate,
        verbose=True,
        batch_size=batch_size,
    )
    
    return cnn

# %% [markdown]
# ## Define Experiment
# %%
# %%
class Experiment:
    def __init__(self, experiment):

        experiment_metadata = {
            "impulse": [load_impulse_Xy, "./timeseries_1d/"],
            "hvbar": [load_hvbar_Xy, "./hvbar/"],
            "circle": [load_circle_Xy, "./circle_experiment/"],
            "mnist": [load_mnist_Xy, "./mnist/"],
            "cifar10": [load_cifar10, "./cifar10/"],
        }
        # experiment = 'mnist'
        self.experiment = experiment
        self.num_runs = 1
        self.save_dir = "optimization_results"

        self.ncores = 45
        self.n_est = 1000
        self.normalize=True

        self.data_loader, self.root_dir = experiment_metadata[self.experiment]
        self.ns = np.array([10, 50, 100, 200, 400, 1000, 2000])

        ## Load Data and Params
        if experiment == "mnist":
            self.ns = [100, 200, 500, 1000, 2500, 5000, 10000, 30000, 60000]
            (
                self.X_train,
                self.y_train,
                self.X_test,
                self.y_test,
                self.size_dict,
            ) = get_mnist(self.ns)
        elif experiment == 'cifar10':
            self.ns = [100, 500, 1000, 10000, 25000, 50000]
            (
                self.X_train,
                self.y_train,
                self.X_test,
                self.y_test,
                self.size_dict,
            ) = get_cifar10(self.ns)
        else:
            (
                self.X_train,
                self.y_train,
                self.X_test,
                self.y_test,
                self.size_dict,
            ) = get_data(self.experiment, self.data_loader, self.ns)

        self.imageHeight = self.size_dict["height"]
        self.imageWidth = self.size_dict["width"]

        (
            self.patch_height_maxs,
            self.patch_height_mins,
            self.patch_width_maxs,
            self.patch_width_mins,
            self.wrap,
        ) = get_mf_params(self.experiment)

# %% [markdown]
# ## Optimize

# %%
## Run Optimization
    def optimize(self):
        optimize(
            imageWidth=self.imageWidth,
            imageHeight=self.imageHeight,
            patch_height_maxs=self.patch_height_maxs,
            patch_width_maxs=self.patch_width_maxs,
            patch_height_mins=self.patch_height_mins,
            patch_width_mins=self.patch_width_mins,
            wrap=self.wrap,
            X=self.X_train[:400],
            y=self.y_train[:400],
            root_dir=self.root_dir,
            ncores=self.ncores,
            n_est=self.n_est,
            save_dir=self.save_dir,
        )


    # %% [markdown]
    # ## Evaluate sklearn algorithms and MORF


    def eval_classifiers(self, use_clf, tag=None, eval_test_time=False, save_models=False, proj_mat = "S-RerF"):
        ## Load default classifiers
        names, classifiers = get_classifiers(
            width=self.imageWidth,
            height=self.imageHeight,
            ncores=self.ncores,
            n_est=self.n_est,
            projection_matrix=proj_mat,
            clf=use_clf,
        )
        ## Load CNN
        names.update({"CNN": "#cab2d6"})
        classifiers.append(get_cnn(self.experiment, self.size_dict))

        ## Load best params
        param_dict = defaultdict(dict)
        for f in glob(str(Path(self.root_dir) / self.save_dir / "*.pkl")):
            with open(f, "rb") as handle:
                d = pickle.load(handle)
                param_dict[d["name"]] = d["params"]
        if self.experiment == 'cifar10':
            param_dict = defaultdict(dict)
            param_dict['MF'] = {
                'patch_height_max':4,
                'patch_width_max':4,
                'patch_height_min':1,
                'patch_width_min':1,
                'max_features':10000,
            }
            param_dict['SPORF'] = {
                'max_features':10000,
            }

        # %%
        ## Update to best parameters
        ## Only optimize MF parameters, keep defaults for others
        if use_clf is not None:
            classifiers = [
                clf.set_params(**param_dict[name]) if name == 'MF' else clf for clf, name in zip(classifiers, names) if name==use_clf
            ]
            if len(classifiers) > 1:
                sys.exit(1)
            names = {use_clf:names[use_clf]}
        else:
            classifiers = [
                clf.set_params(**param_dict[name]) if name == 'MF' else clf for clf, name in zip(classifiers, names)
            ]

        if eval_test_time:
            classifiers = [
                clf.fit(self.X_train, self.y_train) for clf in classifiers
            ]
        if eval_test_time:
            self.num_runs = 15

        # %%
        # Train each classifier on each dataset size, then test
        ## Prep output file:
        if tag is None:
            tag = ''
        if use_clf is not None:
            tag += f"_{use_clf}"
        if eval_test_time:
            tag += "_test_time"
        write_path = Path(self.root_dir) / f"{self.experiment}_results_{tag}.csv"
        with open(write_path, "w") as f:
            f.write("classifier,n,Lhat,trainTime,testTime,iterate\n")

        runList = [
            (n, clf, run)
            for n in self.ns
            for clf in zip(classifiers, [key for key in names])
            for run in range(self.num_runs)
        ]

        for n, clf, iteration in tqdm(runList):                
            trainStartTime = time.time()
            if self.normalize:
                scaler = StandardScaler()
                scaler = scaler.fit(self.X_train[:n])
            if not eval_test_time:
                clf[0].fit(scaler.transform(self.X_train[:n]), self.y_train[:n])
            trainEndTime = time.time()
            trainTime = trainEndTime - trainStartTime

            if save_models:
                if not os.path.exists(Path(self.root_dir) / 'trained_models'):
                    os.makedirs(Path(self.root_dir) / 'trained_models')
                with open(Path(self.root_dir) / 'trained_models' / f'{clf[1].replace(" ", "")}_{iteration}_{n}.pkl', 'wb') as f:
                    pickle.dump(clf[0], f, pickle.HIGHEST_PROTOCOL)
            #model_bytes = len(pickle.dumps(clf[0], pickle.HIGHEST_PROTOCOL))

            testStartTime = time.time()
            if eval_test_time:
                out = clf[0].predict(self.X_train[:n])
            else:
                out = clf[0].predict(scaler.transform(self.X_test))
            testEndTime = time.time()
            testTime = testEndTime - testStartTime

            if eval_test_time:
                lhat = np.mean(np.not_equal(out, self.y_train[:n]).astype(int))
            else:
                lhat = np.mean(np.not_equal(out, self.y_test).astype(int))

            ####("variable,Lhat,trainTime,testTime,iterate,model_bytes")
            with open(write_path, "a") as f:
                f.write(
                    f"{clf[1]}, {n}, {lhat:2.9f}, {trainTime:2.9f}, {testTime:2.9f}, {iteration}\n"
                )
            gc.collect()

### Main
# Ex. python run_optimize_sklearn_classifiers.py --eval --experiments impulse --clf MF --tag response --save
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--experiments", nargs="+", help="", type=str, required=True)
    parser.add_argument("--clf", nargs="+", help="", type=str, default=None)
    parser.add_argument("--optimize", help="", action="store_true")
    parser.add_argument("--eval", help="", action="store_true")
    #parser.add_argument("--cnn", help="", action="store_true")
    parser.add_argument("--testtime", help="", action="store_true")
    parser.add_argument("--tag", help="", type=str, default=None)
    parser.add_argument("--save", help="", action="store_true")
    parser.add_argument("--proj", help="", type=str, default="S-RerF")
    args = parser.parse_args()
    if args.clf is None:
        clfs = [None]
    else:
        clfs = args.clf
    
    for experiment in args.experiments:
        exp = Experiment(experiment)
        if args.optimize:
                exp.optimize()
        if args.eval:
            for clf in clfs:
                exp.eval_classifiers(
                    clf,
                    tag=args.tag,
                    eval_test_time=args.testtime,
                    save_models=args.save,
                    proj_mat=args.proj
                )
