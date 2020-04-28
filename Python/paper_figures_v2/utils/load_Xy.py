import numpy as np
from .utils import sort_keep_balance
from graspy.simulations import sbm, er_np
from sklearn.datasets import fetch_openml

## Circle
def load_circle_Xy(n1,n2,ns=None,return_meta=True,seed=None):
    ## Sample objects from each class
    np.random.seed(seed)
    n1,n2 = int(n1),int(n2)
    X1 = np.empty((n1,100))
    Y1 = np.full(n1,0)
    for i in range(n1):        
        cir = np.zeros(100)
        
        s1 = np.random.choice(range(0,100)) # Random Location
        l1 = [j%100 for j in range(s1,s1+5)] # Length 5 vector from s1
        
        exclude_idx = set([j%100 for j in range(s1-5,s1+6)])
        s2 = np.random.choice(list(set(range(0,100)).difference(exclude_idx)))
        l2 = [j%100 for j in set(range(s2,s2+5)).difference(set(l1))]
        
        cir[l1] = 1
        cir[l2] = 1
        
        X1[i] = cir
        
    X2 = np.empty((n2,100))
    Y2 = np.full(n2,1)
    for i in range(n2):        
        cir = np.zeros(100)
        
        s1 = np.random.choice(range(0,100)) # Random location   
        l1 = [j%100 for j in range(s1,s1+4)] # Length 4 vector from s1
        
        exclude_idx = set([j%100 for j in range(s1-6,s1+5)])
        s2 = np.random.choice(list(set(range(0,100)).difference(exclude_idx)))
        l2 = [j%100 for j in set(range(s2,s2+6)).difference(set(l1))]
        
        cir[l1] = 1
        cir[l2] = 1
        
        X2[i] = cir

    X = np.vstack([X1,X2])
    y = np.hstack([Y1,Y2])

    if ns is not None:
        idxs = sort_keep_balance(y, ns)
        X = X[idxs]
        y = y[idxs]
    
    if not return_meta:
        return(X,y)
    else:
        return(X,y,{'height':1, 'width':100})

## Load SBMs
def load_sbms_Xy(n1,
                 n2,
                 communities1=[50,50],
                 communities2=[50,50],
                 p1=[[0.2,0.05],[0.05,0.2]],
                 p2=[[0.05,0.2],[0.2,0.05]],
                 ns=None,
                 shuffle=False,
                 seed=None,
                 return_meta=True):
    np.random.seed(seed)
    X = []
    y = []
    for _ in range(n1):
        tmp = sbm(communities1, p1)
        permute = np.arange(sum(communities1))
        np.random.shuffle(permute)
        #tmp = tmp[permute,:][:,permute]
        X.append(tmp)
        y.append(0)

    for _ in range(n2):
        tmp = sbm(communities2, p2)
        permute = np.arange(sum(communities2))
        np.random.shuffle(permute)
        #tmp = tmp[permute,:][:,permute]
        X.append(tmp)
        y.append(1)

    X = np.asarray(X).reshape(len(X),-1)
    y = np.asarray(y)
    # idxs = list(range(len(y)))
    # np.random.shuffle(idxs)

    if ns is not None:
        idxs = sort_keep_balance(y, ns)
        X = X[idxs]
        y = y[idxs]
    
    if not return_meta:
        return(X,y)
    else:
        return(X,y,{'height':sum(communities1), 'width':sum(communities1)})

## Graph signal
def load_graphsignal_Xy(n1,
                         n2,
                         ns,
                         seed=None,
                         return_meta=True):
    np.random.seed(seed)
    X = []
    y = []
    G = er_np(10,0.3)
    signal = [np.exp(-0.1*(j)) for j in range(9)]
    path = np.arange(len(G))
    np.random.shuffle(path)
    path = path[:11]
    for i,j in zip(path[:-1], path[1:]):
        G[i,j] = 1
        G[j,i] = 1

    for _ in range(n1):
        noise = np.random.normal(0,0.5,size=G.shape)
        noise += noise.T
        X.append(np.multiply(G, noise))
        y.append(0)

    for _ in range(n2):
        noise = np.random.normal(0,0.5,size=G.shape)
        noise += noise.T
        G1 = np.multiply(G, noise)
        for i,j,v in zip(path[:-1], path[1:], signal):
            G1[i,j] += v
            G1[j,i] += v
        X.append(G1)
        y.append(1)

    X = np.asarray(X).reshape(len(X),-1)
    y = np.asarray(y)

    if ns is not None:
        idxs = sort_keep_balance(y, ns)
        X = X[idxs]
        y = y[idxs]
    
    if not return_meta:
        return(X,y)
    else:
        return(X,y,{'height':50, 'width':50})

## Correlated timeseries
def load_correlated_ts_Xy(n1,
                          n2,
                          cov1,
                          cov2,
                          tsteps,
                          ns=None,
                          seed=None
    ):
    
    def get_X(cov):
        X = [np.random.multivariate_normal(np.zeros(len(cov)), cov)]
        for _ in range(tsteps-1):
            X = np.random.multivariate_normal(X[-1], cov)
        return np.flatten(np.asarray(X).T)
    y0 = np.zeros(n1)
    X1 = np.random.multivariate_normal(mu, cov2, size=n2)
    y1 = np.ones(n2)

    X = np.hstack((X0,X1))
    y = np.hstack((y0,y1))

    if ns is not None:
        idxs = sort_keep_balance(y, ns)
        X = X[idxs]
        y = y[idxs]
    
    if not return_meta:
        return(X,y)
    else:
        return(X,y,{'height':None, 'width':None})

## Impulse Response
def load_impulse_Xy(n1,
                    n2,
                    ns=None,
                    seed=None,
                    return_meta=True):
    def samp(n, cls=0, separate=False):
        ## Sample n samples from either class 0 or 1
        ## Class 1 has the impulse added
        M0 = 20 # Onset of impulse response
        M1 = 100-M0
        func = lambda x: [np.exp(-0.1*(j-M0)) if j >= M0 else 0.0 for j in x] # Impulse response function
        if cls == 0:
            X = np.full((n,M0+M1),0.0)
            Y = np.full(n,cls)
        elif cls == 1:
            X = np.tile(func(np.arange(0,M0+M1)),reps=(n,1))
            Y = np.full(n,cls)
        
        if separate:
            # White noise
            noise = np.random.normal(0,1,size=(n,M0+M1))
            
            return(X,Y,noise)
        else:
            # White noise
            X += np.random.normal(0,1,size=(n,M0+M1))

            return(X,Y)

    ## Generate a collection of mixed class (shuffled) observations
    np.random.seed(seed)
    X0,Y0 = samp(n1,cls=0)
    X1,Y1 = samp(n2,cls=1)
    X = np.vstack([X0,X1])
    y = np.hstack([Y0,Y1])
    
    if ns is not None:
        idxs = sort_keep_balance(y, ns)
        X = X[idxs]
        y = y[idxs]
    
    if not return_meta:
        return(X,y)
    else:
        return(X,y,{'height':1, 'width':int(len(X[0]))})    

## Load HVBar
def load_hvbar_Xy(
    n1,
    n2,
    ns=None,
    seed=None,
    return_meta=True
    ):

    np.random.seed(seed)

    h = 28; w = 28; lambda_ = 10
    num_bars = np.random.poisson(lam = lambda_, size = n1+n2)

    loc = [np.random.randint(0, h, nbi) for nbi in num_bars]
    
    y = [0,1] * int((n1+n2) / 2)

    X = []
    for i in range(n1+n2):
        temp = np.zeros(h * w).reshape(h, w)
        for j in loc[i]:
            temp[j, :] = 1

        X.append(temp)
            
    for ci in range(len(y)):
        if y[ci] == 1:
            X[ci] = np.transpose(X[ci])

    X = np.asarray(X).reshape(len(X), -1)
    y = np.asarray(y)

    if ns is not None:
        idxs = sort_keep_balance(y, ns)
        X = X[idxs]
        y = y[idxs]
    
    if not return_meta:
        return(X,y)
    else:
        return(X,y,{'height':h, 'width':w})

def load_mnist_Xy(ns=None, return_meta=True):
    X, y = fetch_openml('mnist_784', version=1, return_X_y=True)
    y = y.astype(int)
    X = X.reshape((X.shape[0], -1))

    h = 28; w = 28
    
    if not return_meta:
        return(X,y)
    else:
        return(X,y,{'height':h, 'width':w})