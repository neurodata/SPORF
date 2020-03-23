# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %%
from IPython import get_ipython

# %%
from rerf.rerfClassifier import rerfClassifier
import numpy as np

import matplotlib
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
import seaborn as sns


# %%
def sample_Xy(n=10):
    X = []
    y = []
    
    for i in range(n):
        # Class 0
        X.append(np.asarray([[0,1,0],[0,-1,0],[0,1,0]]).flatten())
        y.append(0)
        
        # Class 1
        X.append(np.asarray([[0,-1,0],[0,1,0],[0,-1,0]]).flatten())
        y.append(1)
    
    return(np.asarray(X), np.asarray(y))


# %%
X,y = sample_Xy(1)


# %%
n_channels = 3
ncols = 3
wmax = 3
wmin = 3
ncores = 1
n_est = 1

# %% [markdown]
# ## MORF Fails

# %%
hmax = 2
hmin = 2


# %%
clf = rerfClassifier(projection_matrix="S-RerF",
                   max_features='auto',
                   n_jobs=ncores,
                    n_estimators=n_est,
                    oob_score=False,
                    random_state=0,
                    image_height=n_channels,
                    image_width=ncols,
                    patch_height_max=hmax,
                    patch_height_min=hmin,
                    patch_width_max=wmax,
                    patch_width_min=wmin
                   )


# %%
clf.fit(X,y)
clf.score(X,y)

# %% [markdown]
# # MORF Succeeds

# %%
hmax = 3
hmin = 3


# %%
clf = rerfClassifier(projection_matrix="S-RerF",
                   max_features='auto',
                   n_jobs=ncores,
                    n_estimators=n_est,
                    oob_score=False,
                    random_state=0,
                    image_height=n_channels,
                    image_width=ncols,
                    patch_height_max=hmax,
                    patch_height_min=hmin,
                    patch_width_max=wmax,
                    patch_width_min=wmin
                   )


# %%
clf.fit(X,y)
clf.score(X,y)


# %% [markdown]
# ## TORF succeeds where MORF fails

# %%
hmax = 2
hmin = 2


# %%
clf = rerfClassifier(projection_matrix="S-RerF",
                   max_features='auto',
                   n_jobs=ncores,
                    n_estimators=n_est,
                    oob_score=False,
                    random_state=0,
                    image_height=n_channels,
                    image_width=ncols,
                    patch_height_max=hmax,
                    patch_height_min=hmin,
                    patch_width_max=wmax,
                    patch_width_min=wmin
                   )


# %%
clf.fit(X,y)
clf.score(X,y)


# %%


