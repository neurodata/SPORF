#%% Imports
import numpy as np
import pandas as pd
from pathlib import Path
import logging
import os
import re
import sys
import h5py

#%% Define paths
datadir = Path('/data/ronan/')
rawdir = datadir / 'raw'
logpath = Path('.')

## Grab filenames
def get_files(path,
              level='(e|n)',
              subject='([0-9]{3})',
              task='(.+?)',
              ftype='csv',
              flag=''):
    files = []
    query = f'^{level}_sub-'
    query += f'{subject}_ses-1_'
    query += f'task-{task}{flag}\.{ftype}'

    for f in os.listdir(path):
        match = re.search(query, f)
        if match:
            groups = match.groups()
            files.append((f, (groups[0], groups[2])))
    return(files)

## Read a csv or h5 file. Meta corresponds to h5_key
def read_file(path, ftype, h5_key=None):
    if ftype == 'csv':
        return(pd.read_csv(path, header = None).to_numpy())
    elif ftype == 'h5':
        h5f = h5py.File(path,'r')
        temp = h5f[h5_key][:]
        h5f.close()
        return(temp)

def load_Xy(data_source=rawdir, tag='', ftype='csv', h5_key=None):
    ## Get filenames for each task, novice vs. experienced
    tasks = ['restingstate', 'openmonitoring', 'compassion']
    levels = ['e', 'n']

    logging.info(f'Pulling data from {data_source}')

    paths = get_files(path=data_source, ftype=ftype, flag=tag)
    X = []
    y = []
    for path,(level,task) in paths:
        temp = read_file(data_source / path, ftype, h5_key=h5_key)
        if temp.shape[1] != 300:
            continue
        X.append(temp.flatten())
        y.append(3*levels.index(level) + tasks.index(task))

    return(np.asarray(X),np.asarray(y))
    

if __name__ == '__main__':
    X,y = load_Xy()
