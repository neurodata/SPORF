import gc
import math
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

import warnings

warnings.simplefilter("ignore")

# %%
def get_data(experiment, data_loader, ns, normalize=True):
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

    scaler = StandardScaler()
    X_train = scaler.fit_transform(X_train)
    X_test = scaler.transform(X_test)

    if ns is not None:
        idxs = sort_keep_balance(y_train, ns)
        X_train = X_train[idxs]
        y_train = y_train[idxs]

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
    else:
        print("ERROR: Please specify experimental patch sizes")

    return (
        patch_height_maxs,
        patch_height_mins,
        patch_width_maxs,
        patch_width_mins,
        wrap,
    )


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
        }
        # experiment = 'mnist'
        self.experiment = experiment
        self.num_runs = 5
        self.save_dir = "optimization_results"

        self.ncores = 50
        self.n_est = 500

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


    def eval_sklearn(self, use_clf):
        # %%
        ## Load default classifiers
        names, classifiers = get_classifiers(
            width=self.imageWidth,
            height=self.imageHeight,
            ncores=self.ncores,
            n_est=self.n_est,
            projection_matrix="S-RerF",
        )

        ## Load best params
        param_dict = {}
        for f in glob(str(Path(self.root_dir) / self.save_dir / "*.pkl")):
            with open(f, "rb") as handle:
                d = pickle.load(handle)
                param_dict[d["name"]] = d["params"]

        # %%
        ## Update to best parameters
        classifiers = [
            clf.set_params(**param_dict[name]) if name == 'MF' else clf for clf, name in zip(classifiers, names)
        ]

        # %%
        # Train each classifier on each dataset size, then test
        ## Prep output file:
        if use_clf is None:
            write_path = Path(self.root_dir) / f"{self.experiment}_results_sklearn.csv"
        else:
            write_path = Path(self.root_dir) / f"{self.experiment}_results_{use_clf}.csv"
        f = open(write_path, "w+")
        f.write("classifier,n,Lhat,trainTime,testTime,iterate\n")
        f.flush()

        runList = [
            (n, clf, run)
            for n in self.ns
            for clf in zip(classifiers, [key for key in names])
            for run in range(self.num_runs)
        ]

        for n, clf, iteration in tqdm(runList):
            if use_clf is not None and clf[1] != use_clf:
                continue
            trainStartTime = time.time()
            clf[0].fit(self.X_train[:n], self.y_train[:n])
            trainEndTime = time.time()
            trainTime = trainEndTime - trainStartTime

            testStartTime = time.time()
            out = clf[0].predict(self.X_test)
            testEndTime = time.time()
            testTime = testEndTime - testStartTime

            lhat = np.mean(np.not_equal(out, self.y_test).astype(int))

            ####("variable,Lhat,trainTime,testTime,iterate")
            f.write(
                f"{clf[1]}, {n}, {lhat:2.9f}, {trainTime:2.9f}, {testTime:2.9f}, {iteration}\n"
            )
            f.flush()

        f.close()


    def eval_cnn(self):
        model_dict = {
            "impulse": [ImpulseModel, (1, self.size_dict["width"])],
            "hvbar": [HVBarModel, (1, self.size_dict["height"], self.size_dict["width"])],
            "circle": [CircleModel, (1, self.size_dict["width"])],
            "mnist": [MNISTModel, (1, self.size_dict["height"], self.size_dict["width"])],
        }
        ## Setup for run
        names = {"CNN": "#cab2d6"}
        model, shape = model_dict[experiment]
        # Hyperparameters
        num_epochs = 100
        learning_rate = 0.001
        batch_size=-1

        classifiers = [
            CNN(
                model=model,
                shape=shape,
                num_epochs=num_epochs,
                learning_rate=learning_rate,
                verbose=False,
                batch_size=batch_size,
            )
        ]

        # Train each classifier on each dataset size, then test
        ## Prep output file:
        np.random.seed(1234)

        write_path = Path(self.root_dir) / f"{self.experiment}_results_cnn.csv"
        f = open(write_path, "w+")
        f.write("classifier,n,Lhat,trainTime,testTime,iterate\n")
        f.flush()

        runList = [
            (n, clf, run)
            for n in self.ns
            for clf in zip(classifiers, [key for key in names])
            for run in range(self.num_runs)
        ]

        for n, clf, iteration in tqdm(runList):
            trainStartTime = time.time()
            clf[0].fit(self.X_train[:n], self.y_train[:n])
            trainEndTime = time.time()
            trainTime = trainEndTime - trainStartTime

            testStartTime = time.time()
            yhat = clf[0].predict(self.X_test)
            testEndTime = time.time()
            testTime = testEndTime - testStartTime

            lhat = np.mean(np.not_equal(yhat, self.y_test).astype(int))

            ####("variable,Lhat,trainTime,testTime,iterate")
            f.write(
                f"{clf[1]}, {n}, {lhat:2.9f}, {trainTime:2.9f}, {testTime:2.9f}, {iteration}\n"
            )
            f.flush()

        f.close()


### Main
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("experiment", help="", type=str)
    parser.add_argument("--clf", help="", type=str, default=None)
    parser.add_argument("--optimize", help="", action="store_true")
    parser.add_argument("--sklearn", help="", action="store_true")
    parser.add_argument("--cnn", help="", action="store_true")
    args = parser.parse_args()
    experiment = args.experiment
    exp = Experiment(experiment)
    if args.optimize:
        exp.optimize()
    if args.sklearn:
        exp.eval_sklearn(args.clf)
    if args.cnn:
        exp.eval_cnn()

# %% [markdown]
# ## Plot

# %%
