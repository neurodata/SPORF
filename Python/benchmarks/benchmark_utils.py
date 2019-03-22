import openml
from RerF import fastRerF, fastPredict
from sklearn.ensemble import RandomForestClassifier, ExtraTreesClassifier
import matplotlib.pyplot as plt
from tqdm import tqdm_notebook as tqdm
import numpy as np
import time

def fit_RerF(X,
             y,
             train_test_splits, 
             forest_type='binnedBaseRerF', 
             forest_kwargs=None
    ):
    # I assume data has been preprocessed
    # if num_trees is None:
    #     num_trees = 10
    # if acorn is None:
    #     acorn = np.random.randint(100000)
    # np.random.seed(acorn)
    
    train_idx = train_test_splits[0]
    test_idx = train_test_splits[1]
    X_train, X_test = X[train_idx], X[test_idx]
    y_train, y_test = y[train_idx], y[test_idx]
    
    start = time.time()
    rerf = fastRerF(X_train, y_train, forestType=forest_type, **forest_kwargs)
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
                forest_kwargs=None
    ):
    # I assume the data has been preprocessed
    # if num_trees is None:
    #     num_trees = 10
    # if acorn is None:
    #     acorn = np.random.randint(100000)
    # np.random.seed(acorn)
    
    train_idx = train_test_splits[0]
    test_idx = train_test_splits[1]
    
    X_train, X_test = X[train_idx], X[test_idx]
    y_train, y_test = y[train_idx], y[test_idx]
    
    if forest_type == 'RandomForest':
        model = RandomForestClassifier(**forest_kwargs)
    elif forest_type == 'ExtraTrees':
        model = ExtraTreesClassifier(**forest_kwargs)
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
              n_iterations=10,
              preprocess=False,
              train_test_splits = None,
              rerfs=['binnedBaseRerF'], 
              rerfs_kwargs=[None],
              sklearns=['RandomForest'],
              sklearns_kwargs=[None],
              acorn=None
    ):
    # if acorn is None:
    #     acorn = np.random.randint(100000)
    # if num_trees is None:
    #     num_trees = 10
    # np.random.seed(acorn)
    
    task = openml.tasks.get_task(oml_task_id)
    X, y = task.get_X_and_y()
    
    if len(rerfs) > len(rerfs_kwargs):
    	if len(rerfs_kwargs) == 1:
    	    rerfs_kwargs = [rerfs_kwargs[0] for model in rerfs]
    	else:
    	    raise ValueError('bad rerfs_kwargs')

    if len(sklearns) > len(sklearns_kwargs):
    	if len(sklearns_kwargs) == 1:
    	    sklearns_kwargs = [sklearns_kwargs[0] for model in sklearns]
    	else:
    	    raise ValueError('bad sklearns_kwargs')


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
            rerf_results[i,:,j]=fit_RerF(X, y, train_test_splits, forest_type=model, forest_kwargs=rerfs_kwargs[i])
        
    sklearn_results = np.zeros(shape=(len(rerfs), 4, n_iterations))
    for i, model in enumerate(sklearns):
        for j in range(n_iterations):
            sklearn_results[i,:,j]=fit_sklearn(X, y, train_test_splits, forest_type=model, forest_kwargs=sklearns_kwargs[i])
    
    return np.concatenate((rerf_results, sklearn_results))

def benchmark_plot(results, # Assumed to be an M x F x 4 x n_iterations array,
                   all_forests = ['binnedRerF, RandomForest'],
                   titles=['test accuracy', 'train accuracy', 'train time', 'predict time'],
                   param='n_trees',
                   param_values=None,
                   task_id=-1
    ):
    M, F, K, n_iterations = results.shape
    averages = np.mean(results, axis=3)
    data = [[[] for _ in range(4)] for __ in range(F)]


    for i in range(M):
        for j in range(len(data)):
            for k in range(len(data[j])):
                data[j][k].append(averages[i][j][k])
                
    K = len(titles)
    axes = [plt.subplots(1,1) for _ in range(K)]

    if param_values is None:
    	param_values=range(1, M + 1)
    
    for i, forest in enumerate(all_forests):
        for j in range(len(axes)):
            axes[j][1].plot(param_values, data[i][j], label=forest)
        
    for i, title in enumerate(titles):
        axes[i][1].legend()
        if task_id == -1:
            axes[i][1].set_title(title)#+ ' OpenML task %s'%(task_id))
        else:
            axes[i][1].set_title(title + ' OpenML task %s'%(task_id))
        axes[i][1].set_xlabel(param)
        
    return