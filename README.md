# [Randomer Forest](https://arxiv.org/pdf/1506.03410v2.pdf "arxiv link to RerF paper")

[![CRAN Status Badge](http://www.r-pkg.org/badges/version/rerf)](http://cran.r-project.org/web/packages/rerf)

- [Repo contents](#repo-contents)
- [Description](#description)
- [Hardware Requirements](#hardware-requirements)
- [Software Dependencies](#software-dependencies)
- [Installation](#installation)
- [How to Use](#how-to-use)

## Repo Contents
- [**R**](https://github.com/neurodata/R-RerF/tree/master/R): `R` building blocks for user interface code. Internally called by user interface.
- [**man**](https://github.com/fneurodata/R-RerF/tree/master/man): Package documentation
- [**src**](https://github.com/neurodata/R-RerF/tree/master/src): C++ functions called from within R
- [**travisTest**](https://github.com/fneurodata/R-RerF/tree/master/travisTest): Travis CI tests

## Description
Randomer Forest (RerF) is a generalization of the Random Forest (RF) algorithm. RF partitions the input (feature) space via a series of recursive binary hyperplanes. Hyperplanes are constrained to be axis-aligned. In other words, each partition is a test of the form X<sub>i</sub> > t, where t is a threshold and X<sub>i</sub> is one of p inputs (features) {X<sub>1</sub>, ..., X<sub>p</sub>}. The best axis-aligned split is found by sampling a random subset of the p inputs and choosing the one that best partitions the observed data according to some specified split criterion. RerF relaxes the constraint that the splitting hyperplanes must be axis-aligned. That is, each partition in RerF is a test of the form w<sub>1</sub>X<sub>1</sub> + ... + w<sub>p</sub>X<sub>p</sub> > t. The orientations of hyperplanes are sampled randomly via a user-specified distribution on the coefficients w<sub>i</sub>, although an empirically validated default distribution is provided. Currently only classification is supported. Regression and unsupervised learning will be supported in the future.


## Tested on

- Mac OSX: 10.11 10.12 (Sierra)
- Linux: Ubuntu 16.04, CentOS 6
- Windows: 10

## Hardware Requirements
Any machine with >= 2 GB RAM


## Software Dependencies
- `R`
- `R` packages:
  - `dummies`
  - `compiler`
  - `RcppZiggurat`
  - `parallel`

## Installation
- Installation normally takes ~5-10 minutes
- Non-Windows users install the GNU Scientific Library (libgsl0-dev).
- Windows users install Rtools (https://cran.r-project.org/bin/windows/Rtools/)

### Stable Release from CRAN:
From within R-

```install.packages("rerf")```

### Development Version from Github:
First install the `devtools` package if not currently installed. From within R-  

```install.packages("devtools")```

Next install `rerf` from github.  From within R-  

```devtools::install_github("neurodata/R-Rerf")```

## How to Use

Runtime for the following examples should be < 1 sec on any machine.

### Load the library :
```library(rerf)```

### Create a forest:
To create a forest the minimum data needed is an n by d input matrix (X) and an n length vector of corresponding class labels (Y).  Rows correspond to samples and columns correspond to features.  

```
X <- as.matrix(iris[,1:4])
Y <- iris[[5L]]
forest <- RerF(X, Y, seed = 1L)
```

**Expected output:**

```
> forest$trees[[1]]
$treeMap
 [1]   1   2 -17   3   4  -1  -2   5   8  -3   6   7  -6  -4  -5   9 -16  10 -15
[20]  -7  11  12 -14  13  14  -8  -9 -10  15 -11  16 -12 -13

$CutPoint
 [1]  -0.80  -6.85  -1.90   4.35  -2.75  -5.90   7.55  -2.85 -10.75  -3.35
[11]   3.45  -3.15   4.90   4.60  -3.05   6.40

$ClassProb
      [,1]      [,2]      [,3]
 [1,]    0 1.0000000 0.0000000
 [2,]    0 0.0000000 1.0000000
 [3,]    0 1.0000000 0.0000000
 [4,]    0 0.3333333 0.6666667
 [5,]    0 1.0000000 0.0000000
 [6,]    0 1.0000000 0.0000000
 [7,]    0 0.0000000 1.0000000
 [8,]    0 1.0000000 0.0000000
 [9,]    0 0.0000000 1.0000000
[10,]    0 1.0000000 0.0000000
[11,]    0 0.0000000 1.0000000
[12,]    0 0.0000000 1.0000000
[13,]    0 0.6666667 0.3333333
[14,]    0 0.0000000 1.0000000
[15,]    0 1.0000000 0.0000000
[16,]    0 0.0000000 1.0000000
[17,]    1 0.0000000 0.0000000

$matAstore
 [1]  4 -1  1 -1  1 -1  3  1  2  1  4  1  2 -1  1 -1  1  1  4  1  2 -1  1 -1  3
[26] -1  2 -1  3  1  4 -1  2 -1  3  1  3  1  2 -1  1  1

$matAindex
 [1]  0  2  4  8 12 14 16 20 22 26 28 32 34 36 38 40 42

$ind
NULL

$rotmat
NULL

$rotdims
NULL

$delta.impurity
NULL
```

"forest" is a trained forest which is needed for all other rerf functions.  Additional parameters and more complex examples of training a forest can be found using the help function (```?RerF```)

### Making predictions and determining error rate:
In the example below, trainIdx is used to subset the iris dataset in order to make a training set and a testing set.
```
trainIdx <- c(1:40, 51:90, 101:140)
X <- as.matrix(iris[,1:4])
Y <- iris[[5L]]
forest <- RerF(X[trainIdx, ], Y[trainIdx], num.cores = 1L, rank.transform = TRUE, seed = 1)
# Using a set of samples with unknown classification
predictions <- Predict(X[-trainIdx, ], forest, num.cores = 1L, Xtrain = X[trainIdx, ])
error.rate <- mean(predictions != Y[-trainIdx])
```

**Expected output:**

```
> predictions
 [1] setosa     setosa     setosa     setosa     setosa     setosa    
 [7] setosa     setosa     setosa     setosa     versicolor versicolor
[13] versicolor versicolor versicolor versicolor versicolor versicolor
[19] versicolor versicolor virginica  virginica  virginica  virginica 
[25] virginica  virginica  virginica  virginica  virginica  virginica 
Levels: setosa versicolor virginica

> error.rate
[1] 0
```

If a testing set is not available the error rate of a forest can be determined based on the samples held out-of-bag while training (out-of-bag samples are randomly chosen for each tree in the forest).
```
X <- as.matrix(iris[,1:4])
Y <- iris[[5L]]
forest <- RerF(X, Y, store.oob=TRUE, num.cores = 1L, seed = 1L)
predictions <- OOBPredict(X, forest, num.cores = 1L)
oob.error <- mean(predictions != Y)
```

**Expected output:**

```
> predictions
  [1] setosa     setosa     setosa     setosa     setosa     setosa    
  [7] setosa     setosa     setosa     setosa     setosa     setosa    
 [13] setosa     setosa     setosa     setosa     setosa     setosa    
 [19] setosa     setosa     setosa     setosa     setosa     setosa    
 [25] setosa     setosa     setosa     setosa     setosa     setosa    
 [31] setosa     setosa     setosa     setosa     setosa     setosa    
 [37] setosa     setosa     setosa     setosa     setosa     setosa    
 [43] setosa     setosa     setosa     setosa     setosa     setosa    
 [49] setosa     setosa     versicolor versicolor versicolor versicolor
 [55] versicolor versicolor versicolor versicolor versicolor versicolor
 [61] versicolor versicolor versicolor versicolor versicolor versicolor
 [67] versicolor versicolor versicolor versicolor virginica  versicolor
 [73] versicolor versicolor versicolor versicolor versicolor virginica 
 [79] versicolor versicolor versicolor versicolor versicolor virginica 
 [85] versicolor versicolor versicolor versicolor versicolor versicolor
 [91] versicolor versicolor versicolor versicolor versicolor versicolor
 [97] versicolor versicolor versicolor versicolor virginica  virginica 
[103] virginica  virginica  virginica  virginica  versicolor virginica 
[109] virginica  virginica  virginica  virginica  virginica  virginica 
[115] virginica  virginica  virginica  virginica  virginica  versicolor
[121] virginica  virginica  virginica  virginica  virginica  virginica 
[127] virginica  virginica  virginica  virginica  virginica  virginica 
[133] virginica  versicolor virginica  virginica  virginica  virginica 
[139] virginica  virginica  virginica  virginica  virginica  virginica 
[145] virginica  virginica  virginica  virginica  virginica  virginica 
Levels: setosa versicolor virginica

> oob.error
[1] 0.04
```

### Compute similarities:
Computes pairwise similarities between observations. The similarity between two points is defined as the fraction of trees such that two points fall into the same leaf node (i.e. two samples are similar if they consistently show up in the same leaf node).  This function produces an n by n symmetric similarity matrix.  
```  
X <- as.matrix(iris[,1:4])
Y <- iris[[5L]]
forest <- RerF(X, Y, num.cores = 1L, seed = 1L)
sim.matrix <- ComputeSimilarity(X, forest, num.cores = 1L)
```

**Expected output:**

```
> sim.matrix[1, ]
  [1] 1.00 0.94 0.94 0.94 1.00 0.97 0.97 1.00 0.91 0.96 0.99 1.00 0.94 0.91 0.89
 [16] 0.83 0.98 1.00 0.90 1.00 1.00 1.00 0.98 0.97 1.00 0.94 1.00 1.00 1.00 0.94
 [31] 0.96 1.00 0.98 0.90 0.96 0.96 0.93 1.00 0.91 1.00 1.00 0.87 0.93 0.98 0.98
 [46] 0.92 1.00 0.94 0.99 0.97 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.01 0.00 0.00
 [61] 0.01 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00
 [76] 0.00 0.00 0.00 0.00 0.00 0.01 0.01 0.00 0.00 0.00 0.01 0.00 0.00 0.00 0.00
 [91] 0.00 0.00 0.00 0.01 0.00 0.00 0.00 0.00 0.02 0.00 0.00 0.00 0.00 0.00 0.00
[106] 0.00 0.00 0.00 0.00 0.01 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.01 0.00 0.00
[121] 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00
[136] 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00
```

### Compute tree strengths and correlations:
Computes estimates of tree strength and correlation according to the definitions in Breiman's 2001 Random Forests paper.  
```
trainIdx <- c(1:40, 51:90, 101:140)
X <- as.matrix(iris[,1:4])
Y <- iris[[5L]]
forest <- RerF(X[trainIdx, ], Y[trainIdx], num.cores = 1L, seed = 1L)
predictions <- Predict(X[-trainIdx, ], forest, num.cores = 1L, aggregate.output = FALSE)
scor <- StrCorr(predictions, Y[-trainIdx])
```

**Expected output:**

```
> scor
$s
[1] 0.9413333

$rho
[1] 0.8451606
```

### Compute feature (projection) importance (this feature is not available in the current CRAN release):
Computes the Gini importance for all of the unique projections used to split the data. The returned value is a list with members imp and proj. The member imp is a numeric vector of feature importances sorted in decreasing order. The member proj is a list the same length as imp of vectors specifying the split projections corresponding to the values in imp. The projections are represented by the vector such that the odd numbered indices indicate the canonical feature indices and the even numbered indices indicate the linear coefficients. For example a vector (1,-1,4,1,5,-1) is the projection -X1 + X4 - X5. **Note**: it is highly advised to run this only when the splitting features (projections) have unweighted coefficients, such as for the default setting or for RF.
```
X <- as.matrix(iris[, 1:4]) # feature matrix
Y <- iris$Species # class labels
p <- ncol(X) # number of features in the data
d <- ceiling(sqrt(p)) # number of features to sample at each split

# Here we specify that we want to run the standard random forest algorithm and we want to store the decrease in impurity at each split node. The latter option is required in order to compute Gini feature importance.

forest <- RerF(as.matrix(iris[, 1:4]), iris[[5L]], mat.options = list(p, d, "rf", NULL), num.cores = 1L, store.impurity = TRUE, seed = 1L)
feature.imp <- FeatureImportance(forest, num.cores = 1L)
```

**Expected output:**

```
> feature.imp
$imp
[1] 4455.7292 4257.6306  861.6474  178.5267

$proj
$proj[[1]]
[1] 3 1

$proj[[2]]
[1] 4 1

$proj[[3]]
[1] 1 1

$proj[[4]]
[1] 2 1
```
### Train Structured RerF (S-RerF) for image classification:

S-RerF samples and evaluates a set of random features at each split node, where each feature is defined as a random linear combination of intensities of pixels  contained in a contiguous patch within an image. Thus, the generated features exploit local structure inherent in images.

```
data(mnist)
# p is number of dimensions, d is the number of random features to evaluate, iw is image width, ih is image height, patch.min is min width of square patch to sample pixels from, and patch.max is the max width of square patch
p <- ncol(mnist$Xtrain)
d <- ceiling(sqrt(p))
iw <- sqrt(p)
ih <- iw
patch.min <- 1L
patch.max <- 5L
forest <- RerF(mnist$Xtrain, mnist$Ytrain, num.cores = 1L, mat.options = list(p, d, "image-patch", iw, ih, patch.min, patch.max), seed = 1L)
predictions <- Predict(mnist$Xtest, forest, num.cores = 1L)
error.rate <- mean(predictions != mnist$Ytest)
```

**Expected output:**

```
> error.rate
[1] 0.0544
```
### Train Structured RerF (S-RerF) for spike train inference:

Similar to S-RerF for image classification except now in the Spike Train setting. 500 samples were stimulated from the following AR(2) model:
$$
c_t = \sum_{i=1}^2 \gamma_i c_{t-i} + s_t, \ \ \ s_t \sim Poisson(0.01) \\
y_t = a \ c_t + \epsilon_t, \ \ \ \ \epsilon_t \sim \mathcal{N}(0, 1)
$$
whre $\gamma_1 = 1.7, \gamma_2 = -0.712$, $a = 1$. We sampled such that the were an equal number of spikes and non-spikes in the datasets. S-RerF was trained on these samples by computing local feature patches across the time series windows.

```
ts.train <- read.csv('calcium-spike_train.csv', header=FALSE)
ts.test <- read.csv('calcium-spike_test.csv', header=FALSE)
ts.train$X <- ts.train[,1:(ncol(ts.train)-1)]
ts.train$Y <- ts.train[,ncol(ts.train)]
ts.test$X <- ts.test[,1:(ncol(ts.test)-1)]
ts.test$Y <- ts.test[,ncol(ts.test)]

# p is number of dimensions, d is the number of random features to evaluate, patch.min is min width of a time series patch to sample, and patch.max is the max width of the patch.
p <- ncol(ts.train$X)
d <- ceiling(sqrt(p))
patch.min <- 1L
patch.max <- 5L
forest <- RerF(ts.train$X, ts.train$Y, num.cores = 1L, mat.options = list(p, d, "ts-patch", patch.min, patch.max), seed = 1L)
predictions <- Predict(ts.test$X, forest, num.cores = 1L)
error.rate <- mean(predictions != ts.test$Y)
```

**Expected output**

```
> error.rate
[1] 0.262
```



