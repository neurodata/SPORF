import openml
from RerF import fastRerF, fastPredict
from sklearn.ensemble import RandomForestClassifier, ExtraTreesClassifier
from tqdm import tqdm_notebook as tqdm
import numpy as np
import time

def fit_RerF(X,
             y,
             train_test_splits, 
             forest_type = 'binnedBaseRerF', 
             num_trees = None,
             num_cores = 8, 
             acorn = None
    ):
    # I assume data has been preprocessed
    if num_trees is None:
        num_trees = 10
    if acorn is None:
        acorn = np.random.randint(100000)
    np.random.seed(acorn)
    
    train_idx = train_test_splits[0]
    test_idx = train_test_splits[1]
    X_train, X_test = X[train_idx], X[test_idx]
    y_train, y_test = y[train_idx], y[test_idx]
    
    start = time.time()
    rerf = fastRerF(X_train, y_train, forestType=forest_type, trees=num_trees, numCores=num_cores, seed = acorn)
    end_train = time.time()
    
    y_test_hat = fastPredict(X_test, rerf)
    end_predict = time.time()
    test_accuracy = np.sum(y_test_hat == y_test)/len(y_test)
    
    y_train_hat = fastPredict(X_train, rerf)
    train_accuracy = np.sum(y_train_hat == y_train)/len(y_train)
    
    train_time = end_train - start
    predict_time = end_predict - end_train
    return test_accuracy, train_accuracy, train_time, predict_time

def fit_sklearn(X,
                y,
                train_test_splits, 
                forest_type='RandomForest',
                num_trees = None, 
                num_cores = 8, 
                acorn = None
    ):
    # I assume the data has been preprocessed
    if num_trees is None:
        num_trees = 10
    if acorn is None:
        acorn = np.random.randint(100000)
    np.random.seed(acorn)
    
    train_idx = train_test_splits[0]
    test_idx = train_test_splits[1]
    
    X_train, X_test = X[train_idx], X[test_idx]
    y_train, y_test = y[train_idx], y[test_idx]
    
    if forest_type == 'RandomForest':
        model = RandomForestClassifier(n_estimators=num_trees, n_jobs=num_cores)
    elif forest_type == 'ExtraTrees':
        model = ExtraTreesClassifier(n_estimators=num_trees, n_jobs=num_cores)
    else:
        raise ValueError('model %s not implemented'%(forest_type))
        
    start = time.time()
    
    model.fit(X_train, y_train)
    
    end_train = time.time()
    
    y_test_hat = model.predict(X_test)
    
    end_predict = time.time()
    test_accuracy = np.sum(y_test_hat == y_test)/len(y_test)
    
    y_train_hat = model.predict(X_train)
    train_accuracy = np.sum(y_train_hat == y_train)/len(y_train)
    
    train_time = end_train - start
    predict_time = end_predict - end_train
    
    return test_accuracy, train_accuracy, train_time, predict_time

def benchmark(oml_task_id=3, 
              n_iterations = 10,
              preprocess=False,
              train_test_splits = None,
              rerfs=['binnedBaseRerF'], 
              sklearns=['RandomForest'], 
              num_trees = None, 
              num_cores = 8,
              acorn=None
    ):
    if acorn is None:
        acorn = np.random.randint(100000)
    if num_trees is None:
        num_trees = 10
    np.random.seed(acorn)
    
    task = openml.tasks.get_task(oml_task_id)
    X, y = task.get_X_and_y()
    
    if preprocess:
        # TODO
        pass
    
    if train_test_splits is None:
        train_test_splits = task.get_train_test_split_indices()
    else:
        # TODO
        pass
        
    rerf_results = np.zeros(shape=(len(rerfs), 4, n_iterations))
    
    for i, model in enumerate(rerfs):
        for j in range(n_iterations):
            rerf_results[i,:,j]=fit_RerF(X, y, train_test_splits, forest_type=model, num_trees=num_trees, num_cores=num_cores)
        
    sklearn_results = np.zeros(shape=(len(rerfs), 4, n_iterations))
    for i, model in enumerate(sklearns):
        for j in range(n_iterations):
            sklearn_results[i,:,j]=fit_sklearn(X, y, train_test_splits, forest_type=model, num_trees=num_trees, num_cores=num_cores)
    
    return rerf_results, sklearn_results