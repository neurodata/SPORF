import numpy as np
import pandas as pd
from scipy import stats

import matplotlib
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
import seaborn as sns
import glob
import pickle
import string
import time
from tqdm import tqdm
from collections import defaultdict

from pathlib import Path

# Parameters

models = ['MORF', 'RF', 'ResNet50']
dataset = 'cifar10'
key = 'error'

color_dict = {
    "Log. Reg": "#999999",
    "Lin. SVM": "#f781bf",
    "SVM": "#a65628",
    "kNN": "#ffff33",
    "RF": "#ff7f00",
    "MLP": "#984ea3",
    "XGB": "#cc99ff",
    "SPORF": "#4daf4a",
    "MORF": "#e41a1c",
    "ResNet50": "#377eb8",
}

fontsize = 9
legendsize = 7.5
sns.set()
sns.set(
    style="darkgrid",
    rc={
        "font.size": fontsize,
        "figure.titlesize": fontsize,
        "legend.fontsize": fontsize,
        "xtick.labelsize": fontsize,
        "ytick.labelsize": fontsize,
        "axes.labelsize": fontsize,
        "axes.titlesize": fontsize,
    },
)
matplotlib.rcParams["text.latex.preamble"] = [r"\usepackage{amsmath}"]


def unpickle(file):
    with open(file, "rb") as fo:
        results = pickle.load(fo, encoding="bytes")
    return results


def plot_experiment(ax, dataset, model, key):
    files = glob.glob(f"{dataset}/results/{model}*.pkl")
    to_plot = defaultdict(list)
    for f in files:
        results_dict = unpickle(f)
        if key == "error":
            y_hat = results_dict["y_hat"]
            y = results_dict["y"]
            n = results_dict["n"]
            val = 1 - np.mean(y == y_hat)
        to_plot[n].append(val)
    ns, vals = list(zip(*to_plot.items()))
    sort_idx = np.argsort(ns)
    ns = np.asarray(ns)[sort_idx]
    means = np.mean(vals, axis=1)[sort_idx]
    std = np.std(vals, axis=1)[sort_idx]
    ax.errorbar(ns, means, yerr=std, label=model, c=color_dict[model], ls='-')


legend = True
fig, ax = plt.subplots(1, 1, figsize=(6, 3), sharey=False, sharex=False)

for model in models:
    plot_experiment(ax, dataset, model, key=key)

ax.set_xscale("log")

handles, labels = ax.get_legend_handles_labels()
if legend == True:
    lgd = ax.legend(
        handles,
        labels,
        title="Algorithm",
        loc="upper left",
        bbox_to_anchor=(1.04, 1),
        borderaxespad=0,
    )  # , prop={'size': 7.5})#bbox_to_anchor = (0.55,-0.15,1,1), borderaxespad=0)
    bbox = (lgd,)
else:
    bbox = ()

bax = fig.add_subplot(111, frameon=False)

bax.grid(None)
plt.tick_params(labelcolor="none", top=False, bottom=False, left=False, right=False)
# plt.legend(title='Algorithm', loc='upper left' ,bbox_to_anchor = (1.04,1), borderaxespad=0)
plt.ylabel("Misclassification Rate")  # Mean Loss
plt.xlabel("Number of training samples")
plt.tight_layout()
fig.subplots_adjust(wspace=0.12)

fig.savefig(
    f"./{dataset}/figures/sample_size_vs_{key}.pdf", bbox_inches="tight", format="pdf"
)  # bbox_extra_artists=bbox,
plt.show()
