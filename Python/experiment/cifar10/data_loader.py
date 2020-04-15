# Data sourced from https://www.cs.toronto.edu/~kriz/cifar.html
import pickle
import numpy as np
from pathlib import Path
import os
from glob import glob

data_dir = Path('/data/ronan/cifar-10-batches-py')

def unpickle(file):
    with open(file, 'rb') as fo:
        dict = pickle.load(fo, encoding='bytes')
    return dict

def get_train(batches = [1,2,3,4,5]):
    y = []
    X = []
    for i,batch in enumerate(batches):
        dict = unpickle(data_dir / f'data_batch_{batch}')
        X.append(dict[b'data'])
        y.append(dict[b'labels'])

    return (np.concatenate(X, axis=0),np.concatenate(y, axis=0))

def get_test():
    dict = unpickle(data_dir / f'test_batch')
    return(np.asarray(dict[b'data']), np.asarray(dict[b'labels']))