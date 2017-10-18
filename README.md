# R-RerF

[![CRAN Status Badge](http://www.r-pkg.org/badges/version/rerf)](http://cran.r-project.org/web/packages/rerf)

## This is an R implementation of Randomer Forest([RerF](https://arxiv.org/pdf/1506.03410v2.pdf "arxiv link to RerF paper"))

RerF is similar to Random Forest (RF), but, whereas RF bases cutpoints on individual features, RerF projects the samples at each node into multiple different spaces.  These projections create new features which are then used to determine the best cut point based on minimizing impurity.   

## Functions of R-Rerf:
- [x] Train a classification forest (RerF)
- [x] Determine Error Rate of Forest (Predict and OOBPredict)
- [x] Compute Similarities (ComputeSimilarity)
- [x] Compute tree strength and correlation (StrCorr)
- [x] Compute feature importance
- [ ] Unsupervised learning

## Installation:
- Non-Windows users install the GNU Scientific Library (libgsl0-dev).
- Windows users install Rtools (https://cran.r-project.org/bin/windows/Rtools/)
- Install dev-tools.  From within R-  
```install.packages("devtools")```
- Install R-RerF from github.  From within R-  
```devtools::install_github("neurodata/R-Rerf")```

## Use:
###   Load the library :
```library(rerf)```

###   To create a forest:
To create a forest the minimum data needed is an n by d sample matrix (x) and an n length vector of corresponding classes (y).  Each row of x is a sample and each column of x is a feature.  

```forest <- RerF(x,y)```  
  
"forest" is a trained forest which is needed for all other rerf functions.  Additional parameters and more complex examples of training a forest can be found using the help function (```?RerF```)

###   Making predictions and determining error rate:
In the example below, trainIdx is used to subset the iris dataset in order to make a training set and a testing set.
```
trainIdx <- c(1:40, 51:90, 101:140)
X <- as.matrix(iris[,1:4])
Y <- as.numeric(iris[,5])
forest <- RerF(X[trainIdx, ], Y[trainIdx], num.cores = 1L, rank.transform = TRUE)
# Using a set of samples with unknown classification
predictions <- Predict(X[-trainIdx, ], forest, num.cores = 1L, Xtrain = X[trainIdx, ])
error.rate <- mean(predictions != Y[-trainIdx])
```

If a testing set is not available the error rate of a forest can be determined based on the samples held out-of-bag while training (out-of-bag samples are randomly chosen for each tree in the forest).
```
X <- as.matrix(iris[,1:4])
Y <- iris[[5L]]
forest <- RerF(X, Y, store.oob=TRUE, num.cores = 1L)
predictions <- OOBPredict(X, forest, num.cores = 1L)
oob.error <- mean(predictions != Y)
```  
  
###   Compute similarities:
Computes pairwise similarities between observations. The similarity between two points is defined as the fraction of trees such that two points fall into the same leaf node (i.e. two samples are similar if they consistently show up in the same leaf node).  This function produces an n by n symmetric similarity matrix.  
```  
X <- as.matrix(iris[,1:4])
Y <- iris[[5L]]
forest <- RerF(X, Y, num.cores = 1L)
sim.matrix <- ComputeSimilarity(X, forest, num.cores = 1L)
```  
  
###   Compute tree strengths and correlations:
Computes estimates of tree strength and correlation according to the definitions in Breiman's 2001 Random Forests paper.  
```
trainIdx <- c(1:40, 51:90, 101:140)
X <- as.matrix(iris[,1:4])
Y <- iris[[5]]
forest <- RerF(X[trainIdx, ], Y[trainIdx], num.cores = 1L)
predictions <- Predict(X[-trainIdx, ], forest, num.cores = 1L, aggregate.output =     FALSE)
scor <- StrCorr(predictions, Y[-trainIdx])
```

###   Compute feature importance
Computes the Gini importance for all of the unique projections used to split the data. The returned value is a list with members imp and proj. The member imp is a numeric vector of feature importances sorted in decreasing order. The member proj is a list the same length as imp of vectors specifying the split projections corresponding to the values in imp. The projections are represented by the vector such that the odd numbered indices indicate the canonical feature indices and the even numbered indices indicate the linear coefficients. For example a vector (1,-1,4,1,5,-1) is the projection -X1 + X4 - X5. **Note**: it is highly advised to run this only when the splitting features (projections) have unweighted coefficients, such as for the default setting or for RF.
```

X <- as.matrix(iris[, 1:4]) # feature matrix
Y <- iris$Species # class labels
p <- ncol(X) # number of features in the data
d <- ceiling(sqrt(p)) # number of features to sample at each split

forest <- RerF(as.matrix(iris[, 1:4]), iris[[5L]], mat.options = list(p, d, "rf", NULL), num.cores = 1L, store.impurity = TRUE)
feature.imp <- FeatureImportance(forest, num.cores = 1L)
```