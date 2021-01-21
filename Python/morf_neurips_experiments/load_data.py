 Data sourced from https://www.cs.toronto.edu/~kriz/cifar.html
import pickle
import numpy as np
from pathlib import Path
import os
from glob import glob
from utils.utils import plot_data, sort_keep_balance

def load_cifar10(ns, normalize=True, batches = [1,2,3,4,5]):
    data_dir = Path('/data/ronan/cifar-10-batches-py')
    
    y_train = []
    X_train = []
    for i,batch in enumerate(batches):
        dict = _unpickle(data_dir / f'data_batch_{batch}')
        X_train.append(dict[b'data'])
        y_train.append(dict[b'labels'])
        
    test_dict = _unpickle(data_dir / f'test_batch')
    X_test = np.asarray(dict[b'data'])
    y_yest = np.asarray(dict[b'labels']))

    return (
        np.concatenate(X_train, axis=0),
        np.concatenate(y_train, axis=0),
        X_test,
        y_test
    )

def _unpickle(file):
    with open(file, 'rb') as fo:
        dict = pickle.load(fo, encoding='bytes')
    return dict
    