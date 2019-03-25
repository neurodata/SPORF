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

def OpenML_benchmark(oml_task_id=3, 
                  n_iterations=10,
                  preprocess=False,
                  train_test_splits = None,
                  rerfs=['binnedBaseRerF'], 
                  rerfs_kwargs=[None],
                  rerf_param_keyword='trees',
                  sklearns=['RandomForest'],
                  sklearns_kwargs=[None],
                  sklearn_param_keyword='n_estimators',
                  param_values = range(20, 41, 20),
                  verbose=True,
                  acorn=None
    ):

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
        # have preprocess function
        print("preprocessing not implemented yet, going ahead with analysis on unprocessed data")
        pass
    
    if train_test_splits is None:
        train_test_splits = task.get_train_test_split_indices()

    all_forests = np.concatenate((rerfs, sklearns))
    M, F = len(param_values), len(all_forests)

    data = np.zeros(shape=(M, F, 4, n_iterations))

    if verbose:

        for i, param_value in enumerate(tqdm(param_values)):
            temp_rerfs_kwargs = []
            for j, dic in enumerate(rerfs_kwargs):
                temp_rerfs_kwargs.append(dic.copy())
                temp_rerfs_kwargs[j][rerf_param_keyword] = param_value
        
            temp_sklearn_kwargs = []
            for j, dic in enumerate(sklearns_kwargs):
                temp_sklearn_kwargs.append(dic.copy())
                temp_sklearn_kwargs[j][sklearn_param_keyword] = param_value

            rerf_data = np.zeros(shape=(len(rerfs), 4, n_iterations))
            
            for j, model in enumerate(rerfs):
                for k in range(n_iterations):
                    rerf_data[j,:,k]=fit_RerF(X, y, train_test_splits, forest_type=model, forest_kwargs=rerfs_kwargs[j])
                
            sklearn_data = np.zeros(shape=(len(sklearns), 4, n_iterations))
            for j, model in enumerate(sklearns):
                for k in range(n_iterations):
                    sklearn_data[j,:,k]=fit_sklearn(X, y, train_test_splits, forest_type=model, forest_kwargs=sklearns_kwargs[j])
            
            data[i] = np.concatenate((rerf_data, sklearn_data))

    else:
        for i, param_value in enumerate(param_values):
            temp_rerfs_kwargs = []
            for j, dic in enumerate(rerfs_kwargs):
                temp_rerfs_kwargs.append(dic.copy())
                temp_rerfs_kwargs[j][rerf_param_keyword] = param_value
        
            temp_sklearn_kwargs = []
            for j, dic in enumerate(sklearns_kwargs):
                temp_sklearn_kwargs.append(dic.copy())
                temp_sklearn_kwargs[j][sklearn_param_keyword] = param_value

            rerf_data = np.zeros(shape=(len(rerfs), 4, n_iterations))
            
            for j, model in enumerate(rerfs):
                for k in range(n_iterations):
                    rerf_data[j,:,k]=fit_RerF(X, y, train_test_splits, forest_type=model, forest_kwargs=rerfs_kwargs[j])
                
            sklearn_data = np.zeros(shape=(len(sklearns), 4, n_iterations))
            for j, model in enumerate(sklearns):
                for k in range(n_iterations):
                    sklearn_data[j,:,k]=fit_sklearn(X, y, train_test_splits, forest_type=model, forest_kwargs=sklearns_kwargs[j])
            
            data[i] = np.concatenate((rerf_data, sklearn_data))

    averages = np.mean(data, axis=3)
    averages_rearranged = [[[] for _ in range(4)] for __ in range(F)]

    for i in range(M):
        for j in range(F):
            for k in range(4):
                averages_rearranged[j][k].append(averages[i][j][k])

    standard_errors = np.std(data, axis=3, ddof=1)/np.sqrt(n_iterations)
    stds_rearranged = [[[] for _ in range(4)] for __ in range(F)]

    for i in range(M):
        for j in range(F):
            for k in range(4):
                stds_rearranged[j][k].append(standard_errors[i][j][k])

    return np.array(averages_rearranged), np.array(stds_rearranged)

def OpenML_benchmark_plot(results, # Assumed to be an M x F x 4 x n_iterations array,
                        param_values,
                        all_forests = ['binnedRerF, RandomForest'],
                        titles=['test accuracy', 'train accuracy', 'train time', 'predict time'],
                        param='parameter value',
                        task_id=-1,
                        save=False,
                        filename=None
    ):

    if type(results) is not tuple and type(results) is not list:
        avgs = results
        std_errs = np.zeros(results.shape)
    elif len(results) > 3:
        raise ValueError('takes either tuple of (data, std_err) or just data')
    else:
        avgs, std_errs = results
    
    K = len(titles)
    
    colors = ['b', 'g', 'r', 'c', 'm', 'y', 'k', 'w']

    for j in range(K):
        fig, ax = plt.subplots(1,1)
        for i, forest in enumerate(all_forests):
            ax.plot(param_values, 
                avgs[i][j], 
                label=forest, 
                c=colors[i%len(colors)]
            )
            ax.plot(param_values, 
                avgs[i][j] + std_errs[i][j], 
                c=colors[i%len(colors)], 
                ls=':', 
                alpha=0.5
            )
            ax.plot(param_values, 
                avgs[i][j] - std_errs[i][j], 
                c=colors[i%len(colors)], 
                ls=':', 
                alpha=0.5
            )
            ax.fill_between(param_values, 
                avgs[i][j] + std_errs[i][j], 
                avgs[i][j] - std_errs[i][j], 
                where=avgs[i][j] + std_errs[i][j] >= avgs[i][j] - std_errs[i][j], 
                facecolor=colors[i%len(colors)], 
                alpha=0.15,
                interpolate=True)
            # ax.fill_between(param_values, 
            #     avgs[i][j] + std_errs[i][j], 
            #     avgs[i][j] - std_errs[i][j], 
            #     where=avgs[i][j] + std_errs[i][j] <= avgs[i][j] - std_errs[i][j], 
            #     facecolor=colors[i%len(colors)], 
            #     interpolate=True)
        ax.legend()
        if task_id == -1:
            ax.set_title(titles[j])
        else:
            ax.set_title(titles[j] + ' OpenML task %s'%(task_id))
        ax.set_xlabel(param)
        if save:
            if filename is None:
                raise ValueError('please give a filename, keyword "filename"')
            else:
                plt.savefig(filename + '_' + titles[j] + '.png')
    return results