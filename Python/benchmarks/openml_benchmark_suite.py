import numpy as np
import openml
import os

from benchmark_utils import OpenML_benchmark, OpenML_benchmark_plot

from tqdm import tqdm_notebook as tqdm

np.random.seed(1)

dir_path = 'OpenML-CC18_no_preprocess_n_trees'

if not os.path.exists(dir_path):
    os.makedirs(dir_path)

# The number of times to run each algorithm
n_iterations = 10

# RerF types to benchmark, the duplicate is to allow for RerFs with different
# parameters
rerfs = ['binnedBaseRerF', 'rerf', 'rerf']

# RerF parameters
rerf_kwargs = [
    {
    'trees' : 20,
    'numCores' : 8,
    },
    {
    'trees' : 20,
    'numCores' : 8,
    },
    {
    'trees' : 20,
    'mtryMult': 2, # Comment this line out if RerF version does not support mtryMult
    'numCores' : 8,
    }
]

# Keyword of the parameter we are changing/iterating over
rerf_param_keyword='trees'

# Sklearn forest types to benchmark
sklearns = ['RandomForest', 'ExtraTrees']

# If all forests share the same parameters only need to pass single dictionary 
sklearn_kwargs = [{
    'n_estimators' : 20,
    'n_jobs' : 8,
}]

# Sklearn keyword of the parameter we are changing/iterating over
sklearn_param_keyword='n_estimators'

# The prameter values to consider 
param_values = np.arange(20, 301, step=20)

# A list of all forest types (duplicates included)
all_forests = np.concatenate((rerfs, sklearns))

benchmark_suite = openml.study.get_study('OpenML-CC18','tasks')

for task in tqdm(benchmark_suite.tasks):
    temp_task_str = 'task_' + str(task)
    temp_results = OpenML_benchmark(oml_task_id=task, # Select an OpenML task via its task id
              n_iterations=n_iterations,
              train_test_splits = None, # None forces default OpenML train/test splits
              rerfs=rerfs, # A list of random forest types
              rerfs_kwargs=rerf_kwargs, # A list of dictionaries of RerF parameters
              rerf_param_keyword=rerf_param_keyword, # The keyword of parameter we are changing (i.e. 'trees')
              sklearns=sklearns, # A list of sklearn forest types
              sklearns_kwargs=sklearn_kwargs, # A list of dictionaries of Sklearn parameters
              sklearn_param_keyword=sklearn_param_keyword, # The keyword of parameter we are changing (i.e. 'n_estimators')
              param_values = param_values, # A list of values for the parameter we are changing
              verbose=False,
              acorn=None
            )
    OpenML_benchmark_plot(
        temp_results, 
        param_values=param_values,
        all_forests=all_forests, 
        titles=['test_accuracy', 'train_accuracy', 'train_time', 'predict_time'],
        task_id=3, 
        param='n_trees', 
        save=True,
        filename=os.path.join(dir_path, temp_task_str)
    )