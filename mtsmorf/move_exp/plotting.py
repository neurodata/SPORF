from itertools import cycle

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns
from scipy import stats

from sklearn.inspection import permutation_importance
from sklearn.metrics import roc_curve, auc
from sklearn.preprocessing import label_binarize


label_names = {0: "Down", 1: "Right", 2: "Up", 3: "Left"}
colors = cycle(["#26A7FF", "#7828FD", "#FF5126", "#FDF028"])


def mean_confidence_interval(data, confidence=0.95):
    a = 1.0 * np.array(data)
    n = a.shape[0]
    m, se = np.mean(a, axis=0), stats.sem(a, axis=0)
    h = se * stats.t.ppf((1 + confidence) / 2.0, n - 1)
    return m, m - h, m + h


def plot_signal(t, data, title="", ax=None, label="", **plt_kwargs):
    if ax is None:
        ax = plt.gca()

    avg_signal, lower_bound, upper_bound = mean_confidence_interval(data)

    sns.lineplot(t, avg_signal, ax=ax, **plt_kwargs)
    ax.fill_between(t, lower_bound, upper_bound, alpha=0.25, label=label, **plt_kwargs)

    return ax


def plot_signals(epochs, labels, axs=None):
    nchs = len(epochs.ch_names)
    t = epochs.times

    if axs is None:
        fig, axs = plt.subplots(
            dpi=200, nrows=int(np.ceil(nchs / 4)), ncols=4, figsize=(15, 45)
        )

    for i, ch in enumerate(epochs.ch_names):
        ax = axs[i]

        epochs_data = epochs.get_data()
        data = epochs_data[:, i]

        # for each class label
        for j, (label, color) in enumerate(zip(np.unique(labels), colors)):
            if label in [1, 3]:
                continue

            plot_signal(
                t,
                data[labels == label],
                ax=ax,
                label=f"{label_names[label]}",
                color=color,
            )

            ax.legend()
            ax.set(title=f"{ch}", xlabel="Time (s)", ylabel="LFP (mV)")

    return axs


def plot_roc_multiclass(estimator, X, y_true, n_classes, title="", ax=None):

    if ax is None:
        ax = plt.gca()

    y_pred = estimator.predict(X)

    fpr = dict()
    tpr = dict()
    roc_auc = dict()
    lw = 2

    for i in range(n_classes):
        fpr[i], tpr[i], _ = roc_curve(
            label_binarize(y_true, classes=np.arange(n_classes))[:, i],
            label_binarize(y_pred, classes=np.arange(n_classes))[:, i],
        )
        roc_auc[i] = auc(fpr[i], tpr[i])

    for i, color in zip(range(n_classes), colors):
        ax.plot(
            fpr[i],
            tpr[i],
            color=color,
            lw=lw,
            label=f"ROC curve for {label_names[i]} (area = {roc_auc[i]:0.2f})",
        )

    ax.plot([0, 1], [0, 1], "k--", lw=lw)
    ax.set(
        xlim=[0.0, 1.0],
        ylim=[0.0, 1.05],
        xlabel="False Positive Rate",
        ylabel="True Positive Rate",
        title=title,
    )

    ax.legend(loc="lower right")

    return ax


def plot_feature_importances(result, epochs, ax=None):
    nchs = len(epochs.ch_names)
    nsteps = len(epochs.times)

    if ax is None:
        ax = plt.gca()

    feat_importance_means = result["importances_mean"].reshape(nchs, nsteps)
    feat_importance_stds = result["importances_std"].reshape(nchs, nsteps)

    df_feat_importances = pd.DataFrame(feat_importance_means)

    ax = sns.heatmap(
        df_feat_importances,
        annot=True,
        cmap="Blues",
        xticklabels=epochs.times,
        yticklabels=epochs.ch_names,
        ax=ax,
    )

    return ax