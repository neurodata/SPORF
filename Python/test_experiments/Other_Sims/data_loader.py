import numpy as np
from pathlib import Path
import h5py
import math
from sklearn import preprocessing


## Paths
base_dir = Path('/data/ronan/')

def load_data(dset):
    """
    Loads processed X,y data
    """
    if dset in ['ECG200','UWaveGestureLibraryX','Phoneme']:
        test_data = np.genfromtxt(base_dir / f'{dset}/{dset}_TEST.txt')
        train_data = np.genfromtxt(base_dir / f'{dset}/{dset}_TRAIN.txt')

        le = preprocessing.LabelEncoder()

        y_test = test_data[:,0]
        y_test = le.fit_transform(y_test)
        X_test = test_data[:,1:]

        y_train = train_data[:,0]
        y_train = le.fit_transform(y_train)
        X_train = train_data[:,1:]

        return(X_train,y_train,X_test,y_test)