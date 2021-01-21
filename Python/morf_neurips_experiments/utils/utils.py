import numpy as np
import pandas as pd
from scipy import stats
import seaborn as sns
import matplotlib.pyplot as plt
import io
from contextlib import redirect_stdout

def get_forest_params(forest):
    params = {}

    f = io.StringIO()
    with redirect_stdout(f):
        forest.printParameters()
    out = f.getvalue()

    for line in out.splitlines():
        A = line.split(" -> ")
        params[A[0]] = A[1]

    return params

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

def plot_data(data_files,names,title='',save_path=None,figsize=(5,4),se=False,fontsize=11):
    ## Read output file log and summarize data
    if se:
        d1 = pd.DataFrame(columns = ['classifier', 'n', 'Lhat', 'se', 'color'])
    else:
        d1 = pd.DataFrame(columns = ['classifier', 'n', 'Lhat', 'color'])

    k = 0
    for file in data_files:
        data = pd.read_csv(file)
        for ni in np.unique(data['n']):
            for cl in np.unique(data['classifier']):

                tmp = data[np.logical_and(data['classifier'] == cl,data['n'] == ni)][['n', 'Lhat']]

                if se:
                    stde = stats.sem(tmp['Lhat'])
                    d1.loc[k] = [cl] + list(tmp.mean()) + [stde] + [names[cl]]
                else:
                    d1.loc[k] = [cl] + list(tmp.mean()) + [names[cl]]
                k += 1

    ## Plot
    sns.set(); sns.set(style="darkgrid", rc={"font.size":fontsize,"axes.titlesize":fontsize,"axes.labelsize":fontsize})        
    fig, ax = plt.subplots(figsize = figsize)

    for key in names.keys():
        grp = d1[d1['classifier'] == key]
        if len(grp) == 0:
            continue
        if se:
            ax = grp.plot(ax=ax, kind='line', x='n', y='Lhat', label=key, \
                c = names[key], se='se', alpha=0.65)
        else:
            ax = grp.plot(ax=ax, kind='line', x='n', y='Lhat', label=key, \
                    c = names[key], alpha=0.65)
        ax.set_xscale('log')

    lgd = ax.legend(title='Algorithm', loc='upper left',bbox_to_anchor = (1.04,1), borderaxespad=0, frameon=True)
    plt.title(title)
    plt.ylabel('Mean test error')
    plt.xlabel('Number of training samples')
    plt.tight_layout()
    if save_path is not None:
        plt.savefig(save_path,format='pdf')
    plt.show()