import numpy as np
from .utils import sort_keep_balance
from graspy.simulations import sbm

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
        X.append(sbm(communities1, p1))
        y.append(0)

    for _ in range(n2):
        X.append(sbm(communities2, p2))
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
