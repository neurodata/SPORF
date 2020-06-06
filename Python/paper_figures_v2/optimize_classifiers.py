import numpy as np
from pathlib import Path
import h5py
import math
import time
from datetime import datetime
from tqdm import tqdm
import pickle
from glob import glob
from utils.base_classifiers import get_classifiers
from sklearn.model_selection import StratifiedShuffleSplit
from sklearn.model_selection import GridSearchCV

def optimize(
        imageWidth,
        imageHeight,
        patch_height_maxs,
        patch_width_maxs,
        patch_height_mins,
        patch_width_mins,
        wrap,
        X,
        y,
        root_dir='/.',
        ncores=1,
        n_est=500,
        save_dir='optimization_results',
    ):

    cv = 3 # num cross folds to eval accuracy

    names, classifiers = get_classifiers(width=imageWidth,
                                        height=imageHeight,
                                        ncores=ncores,
                                        n_est=n_est,
                                        projection_matrix="S-RerF"
                                        )

    parameters = {
        "Log. Reg": {'C': [0.01,0.1,1,10,100]},
        "Lin. SVM": {'C': [0.01,0.1,1,10,100]},
        "SVM": {
            'C': [0.1,1,10,100],
            'coef0': [0,0.1,1],
            'degree': [2,3],
            'gamma': [0.01,0.001,0.0001],
            'kernel': ['poly','sigmoid']
        },
        "kNN": {'n_neighbors': [3,4,5,6,7], 'p': [1,2,3]},
        "RF": {'max_features': [0.1,0.3,0.5,0.7]},
        "MLP": {
            'activation': ['tanh','relu'],
            'alpha': [0.0001,0.001,0.01,0.1],
            'hidden_layer_sizes': [(100,),(100,100),(50,100,50)],
            'learning_rate': ['constant','adaptive']
        },
        "SPORF": {'max_features': [0.1,0.3,0.5,0.7]},
        "MF": {
            'max_features': ['auto',0.1,0.3,0.5],
            'patch_height_max': patch_height_maxs,
            'patch_height_min': patch_height_mins,
            'patch_width_max': patch_width_maxs,
            'patch_width_min': patch_width_mins,
            'wrap': [wrap],
            },
    }

    save_path = Path(root_dir) / save_dir
    save_path.mkdir(parents=True, exist_ok=True)
    timestamp = datetime.now().strftime('%m-%d-%H:%M')

    for clf, name in tqdm(zip(classifiers, names.keys())):
        if len(glob(str(save_path / f'{name}_*'))) > 1:
            continue

        params = parameters[name]
        gscv = GridSearchCV(clf, params, cv=3 ,n_jobs=ncores, refit=False)
        gscv.fit(X, y)

        results = gscv.cv_results_

        result_keys = ['params','mean_test_score',
                    'mean_fit_time','mean_score_time']
        
        f = open(save_path / f'{name}_{timestamp}_gscv_results.csv', 'w+')
        f.write("classifier,mean_test_score,mean_fit_time,mean_score_time\n")
        f.flush()

        cv_results = [', '.join([str(results[key][i]) for key in result_keys]) for i in range(len(results['params']))]
        for cv_result in cv_results:
            f.write(f'{name}, {cv_result}\n')
            f.flush()

        with open(save_path / f'{name}_{timestamp}_best_params.pkl', 'wb') as handle:
            pickle.dump({'name':name, 'params':gscv.best_params_}, handle, protocol=pickle.HIGHEST_PROTOCOL)
