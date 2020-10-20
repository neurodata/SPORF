Analysis of Neural Data Using RF Variants
-----------------------------------------

Here, we use RF, SPORF, SRerf, and MTMORF to analyze 
the neural data of subjects in a motor task.

Installation
------------
We require the following packages to run experiments:

    numpy
    scipy
    scikit-learn
    mlxtend
    pingouin
    mne
    mne-bids
    pybv
    joblib
    tqdm
    matplotlib
    seaborn
    rerf # built locally

Use `pipenv` with a virtual environment.

    # create venv here if not alreayd made
    python3.8 -m venv .venv

    # install dev packages
    pipenv install --dev
    
    # if dev versions are needed
    pipenv install https://api.github.com/repos/mne-tools/mne-bids/zipball/master
    
    # make packedForest
    cd ../packedForest
    make
    
    # pip install into venv
    cd ../Python
    pipenv run pip install -e .