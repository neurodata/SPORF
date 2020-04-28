import numpy as np
from pathlib import Path
import h5py
import math
import time
from datetime import datetime
from tqdm import tqdm

## Paths
base_dir = Path('/data/ronan/grasp-and-lift-eeg-detection')
data_dir = base_dir / 'processed'
TAG = ''

# Columns name for labels
cols = ['HandStart','FirstDigitTouch',
        'BothStartLoadPhase','LiftOff',
        'Replace','BothReleased']

# Number of subjects
subjects = range(1,13)

# data path
prelag = 150
postlag = 300

def load_data(lags=[150,300], subjects=[1], TAG=''):
    """
    Loads processed X,y data
    """
    X = []
    y = []
    for subject in subjects:
        h5 = h5py.File(data_dir / f'subj{subject}_prelag={lags[0]}_postlag={lags[1]}_{TAG}_Xy.hdf5','r')
        X.append(h5['X'][:])
        y.append(h5['y'][:])
        h5.close()
    X = np.concatenate(X, axis=0)
    y = np.concatenate(y)
    #y = (y > 0).astype(int)
    idxs = [i for i,v in enumerate(y) if v in [0,1]]
    return(X[idxs],y[idxs])
