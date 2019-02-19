Randomer Forest
================

<!-- README.md is generated from README.Rmd. Please edit that file -->

[![CRAN Status
Badge](https://www.r-pkg.org/badges/version/rerf)](https://cran.r-project.org/package=rerf)
[![arXiv
shield](https://img.shields.io/badge/arXiv-1506.03410-red.svg?style=flat)](https://arxiv.org/abs/1506.03410)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1933344.svg)](https://doi.org/10.5281/zenodo.1933344)

  - [Randomer Forest](#randomer-forest)
      - [Repo Contents](#repo-contents)
      - [Description](#description)
      - [Tested on](#tested-on)
      - [Hardware Requirements](#hardware-requirements)
      - [Software Dependencies](#software-dependencies)
      - [Installation](#installation)
  - [Usage](#usage)
      - [Load the library :](#load-the-library)
      - [Create a forest:](#create-a-forest)
      - [Making predictions and determining error
        rate:](#making-predictions-and-determining-error-rate)
      - [Compute similarities:](#compute-similarities)
      - [Compute tree strengths and
        correlations:](#compute-tree-strengths-and-correlations)
      - [Train Structured RerF (S-RerF) for image
        classification:](#train-structured-rerf-s-rerf-for-image-classification)
      - [Unsupervised classification
        (U-RerF)](#unsupervised-classification-u-rerf)

## Repo Contents

  - [**R**](R): `R` building blocks for user interface code. Internally
    called by user interface.
  - [**man**](man): Package documentation
  - [**src**](src): C++ functions called from within R
  - [**tests**](tests): testthat tests

## Description

R-RerF (aka Randomer Forest (RerF), or Random Projection Forests) is a
generalization of the Random Forest (RF) algorithm. RF partitions the
input (feature) space via a series of recursive binary hyperplanes.
Hyperplanes are constrained to be axis-aligned. In other words, each
partition is a test of the form X<sub>i</sub> \> t, where t is a
threshold and X<sub>i</sub> is one of p inputs (features)
{X<sub>1</sub>, …, X<sub>p</sub>}. The best axis-aligned split is found
by sampling a random subset of the p inputs and choosing the one that
best partitions the observed data according to some specified split
criterion. RerF relaxes the constraint that the splitting hyperplanes
must be axis-aligned. That is, each partition in RerF is a test of the
form w<sub>1</sub>X<sub>1</sub> + … + w<sub>p</sub>X<sub>p</sub> \> t.
The orientations of hyperplanes are sampled randomly via a
user-specified distribution on the coefficients w<sub>i</sub>, although
an empirically validated default distribution is provided. Currently
only classification is supported. Regression and unsupervised learning
will be supported in the future.

## Tested on

  - Mac OSX: 10.11 (El Capitan), 10.12 (Sierra), 10.13 (High Sierra)
  - Linux: Ubuntu 16.04 and 17.10, CentOS 6
  - Windows: 10

## Hardware Requirements

Any machine with \>= 2 GB RAM

## Software Dependencies

  - `R (>= 3.3.0)`
  - `R` packages:
      - `dummies`
      - `compiler`
      - `RcppArmadillo`
      - `RcppZiggurat`
      - `parallel`

## Installation

  - Installation normally takes \~5-10 minutes
  - Non-Windows users install the GNU Scientific Library (libgsl0-dev).
  - Windows users install Rtools
    (<https://cran.r-project.org/bin/windows/Rtools/>)

### Stable Release from CRAN:

From within R-

``` r
install.packages("rerf")
```

### Development Version from Github:

First install the `devtools` package if not currently installed. From
within R-

``` r
install.packages("devtools")
```

Next install `rerf` from github. From within R-

``` r
devtools::install_github("neurodata/R-RerF", local = FALSE)
```

-----

# Usage

Runtime for the following examples should be \< 1 sec on any machine.

### Load the library :

``` r
library(rerf)
```

## Create a forest:

To create a forest the minimum data needed is an n by d input matrix (X)
and an n length vector of corresponding class labels (Y). Rows
correspond to samples and columns correspond to features.

``` r
X <- as.matrix(iris[,1:4])
Y <- iris[[5L]]
forest <- RerF(X, Y, seed = 1L, num.cores = 1L)
```

**Expected output**

``` r
forest$trees[[1]]
#> $treeMap
#>  [1]  1  2 -6  3  5 -1  4 -2 -3 -4 -5
#> 
#> $CutPoint
#> [1] -0.80 -6.15  1.85  1.55  0.85
#> 
#> $ClassProb
#>      [,1] [,2] [,3]
#> [1,]    0    1    0
#> [2,]    0    1    0
#> [3,]    0    0    1
#> [4,]    0    0    1
#> [5,]    0    1    0
#> [6,]    1    0    0
#> 
#> $matAstore
#>  [1]  4 -1  1 -1  2 -1  3  1  4  1  1  1  3 -1
#> 
#> $matAindex
#> [1]  0  2  4  8 10 14
#> 
#> $ind
#> NULL
#> 
#> $rotmat
#> NULL
#> 
#> $rotdims
#> NULL
#> 
#> $delta.impurity
#> NULL
```

`forest` is a trained forest which is needed for all other rerf
functions. Additional parameters and more complex examples of training a
forest can be found using the help function (`?RerF`)

## Making predictions and determining error rate:

In the example below, trainIdx is used to subset the iris dataset in
order to make a training set and a testing set.

``` r
trainIdx <- c(1:40, 51:90, 101:140)
X <- as.matrix(iris[,1:4])
Y <- iris[[5L]]
forest <- RerF(X[trainIdx, ], Y[trainIdx], num.cores = 1L, rank.transform = TRUE, seed = 1)
# Using a set of samples with unknown classification
predictions <- Predict(X[-trainIdx, ], forest, num.cores = 1L, Xtrain = X[trainIdx, ])
error.rate <- mean(predictions != Y[-trainIdx])
```

**Expected output**

``` r
predictions
#>  [1] setosa     setosa     setosa     setosa     setosa     setosa    
#>  [7] setosa     setosa     setosa     setosa     versicolor versicolor
#> [13] versicolor versicolor versicolor versicolor versicolor versicolor
#> [19] versicolor versicolor virginica  virginica  virginica  virginica 
#> [25] virginica  virginica  virginica  virginica  virginica  virginica 
#> Levels: setosa versicolor virginica
error.rate
#> [1] 0
```

If a testing set is not available the error rate of a forest can be
determined based on the samples held out-of-bag while training
(out-of-bag samples are randomly chosen for each tree in the forest).

``` r
X <- as.matrix(iris[,1:4])
Y <- iris[[5L]]
forest <- RerF(X, Y, store.oob=TRUE, num.cores = 1L, seed = 1L)
predictions <- OOBPredict(X, forest, num.cores = 1L)
oob.error <- mean(predictions != Y)
```

**Expected output**

``` r
predictions
#>   [1] setosa     setosa     setosa     setosa     setosa     setosa    
#>   [7] setosa     setosa     setosa     setosa     setosa     setosa    
#>  [13] setosa     setosa     setosa     setosa     setosa     setosa    
#>  [19] setosa     setosa     setosa     setosa     setosa     setosa    
#>  [25] setosa     setosa     setosa     setosa     setosa     setosa    
#>  [31] setosa     setosa     setosa     setosa     setosa     setosa    
#>  [37] setosa     setosa     setosa     setosa     setosa     setosa    
#>  [43] setosa     setosa     setosa     setosa     setosa     setosa    
#>  [49] setosa     setosa     versicolor versicolor versicolor versicolor
#>  [55] versicolor versicolor versicolor versicolor versicolor versicolor
#>  [61] versicolor versicolor versicolor versicolor versicolor versicolor
#>  [67] versicolor versicolor versicolor versicolor virginica  versicolor
#>  [73] virginica  versicolor versicolor versicolor versicolor virginica 
#>  [79] versicolor versicolor versicolor versicolor versicolor virginica 
#>  [85] versicolor versicolor versicolor versicolor versicolor versicolor
#>  [91] versicolor versicolor versicolor versicolor versicolor versicolor
#>  [97] versicolor versicolor versicolor versicolor virginica  virginica 
#> [103] virginica  virginica  virginica  virginica  versicolor virginica 
#> [109] virginica  virginica  virginica  virginica  virginica  virginica 
#> [115] virginica  virginica  virginica  virginica  virginica  versicolor
#> [121] virginica  virginica  virginica  virginica  virginica  virginica 
#> [127] virginica  virginica  virginica  virginica  virginica  virginica 
#> [133] virginica  versicolor virginica  virginica  virginica  virginica 
#> [139] virginica  virginica  virginica  virginica  virginica  virginica 
#> [145] virginica  virginica  virginica  virginica  virginica  virginica 
#> Levels: setosa versicolor virginica
oob.error
#> [1] 0.04666667
```

## Compute similarities:

Computes pairwise similarities between observations. The similarity
between two points is defined as the fraction of trees such that two
points fall into the same leaf node (i.e. two samples are similar if
they consistently show up in the same leaf node). This function produces
an n by n symmetric similarity matrix.

``` r
X <- as.matrix(iris[,1:4])
Y <- iris[[5L]]
forest <- RerF(X, Y, num.cores = 1L, seed = 1L)
sim.matrix <- ComputeSimilarity(X, forest, num.cores = 1L)
```

**Expected output**

``` r
sim.matrix[1, ]
#>   [1] 1.000 0.958 0.962 0.958 0.996 0.948 0.970 0.992 0.936 0.966 0.964
#>  [12] 0.986 0.958 0.938 0.788 0.746 0.954 1.000 0.794 0.994 0.994 0.994
#>  [23] 0.982 0.976 0.984 0.962 0.988 1.000 0.998 0.962 0.964 0.992 0.954
#>  [34] 0.848 0.964 0.970 0.878 0.994 0.938 0.998 0.992 0.884 0.948 0.984
#>  [45] 0.976 0.954 0.992 0.960 0.968 0.972 0.000 0.000 0.000 0.000 0.000
#>  [56] 0.000 0.000 0.008 0.000 0.000 0.002 0.000 0.000 0.000 0.000 0.000
#>  [67] 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000
#>  [78] 0.000 0.000 0.000 0.002 0.004 0.000 0.000 0.008 0.000 0.000 0.000
#>  [89] 0.000 0.000 0.000 0.000 0.000 0.002 0.000 0.000 0.000 0.000 0.014
#> [100] 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000
#> [111] 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.002 0.000 0.000 0.000
#> [122] 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.002
#> [133] 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000
#> [144] 0.000 0.000 0.000 0.000 0.000 0.000 0.000
```

## Compute tree strengths and correlations:

Computes estimates of tree strength and correlation according to the
definitions in Breiman’s 2001 Random Forests paper.

``` r
set.seed(24)
nsamp <- 30 ## number of training samples per species
trainIdx <- vapply(list(1:50, 51:100, 101:150), sample, outer(1,1:nsamp), size = nsamp)
X <- as.matrix(iris[,1:4])
Y <- iris[[5L]]
forest <- RerF(X[trainIdx, ], Y[trainIdx], num.cores = 1L, seed = 1L)
predictions <- Predict(X[-trainIdx, ], forest, num.cores = 1L, aggregate.output = FALSE)
scor <- StrCorr(predictions, Y[-trainIdx])
```

**Expected output**

``` r
scor
#> $s
#> [1] 0.8488667
#> 
#> $rho
#> [1] 0.4747382
```

### Compute feature (projection) importance (DEV version only):

Computes the Gini importance for all of the unique projections used to
split the data. The returned value is a list with members imp and
features. The member imp is a numeric vector of feature importances
sorted in decreasing order. The member features is a list the same
length as imp of vectors specifying the split projections corresponding
to the values in imp. The projections are represented by the vector such
that the odd numbered indices indicate the canonical feature indices and
the even numbered indices indicate the linear coefficients. For example
a vector (1,-1,4,1,5,-1) is the projection -X1 + X4 - X5. **Note**: it
is highly advised to run this only when the splitting features
(projections) have unweighted coefficients, such as for the default
setting or for RF.

``` r
X <- as.matrix(iris[, 1:4]) # feature matrix
Y <- iris$Species # class labels
p <- ncol(X) # number of features in the data
d <- ceiling(sqrt(p)) # number of features to sample at each split

# Here we specify that we want to run the standard random forest algorithm and we want to store the decrease in impurity at each split node. The latter option is required in order to compute Gini feature importance.

forest <- RerF(as.matrix(iris[, 1:4]), iris[[5L]], FUN = RandMatRF,
               paramList = list(p = p, d = d), num.cores = 1L,
               store.impurity = TRUE, seed = 1L)

feature.imp <- FeatureImportance(forest, num.cores = 1L, type = "R")
#> Message: Computing feature importance for RandMatRF.
```

**Expected output**

``` r
feature.imp
#> $imp
#> [1] 23549.727 20799.581  4617.132  1026.042
#> 
#> $features
#> $features[[1]]
#> [1] 4 1
#> 
#> $features[[2]]
#> [1] 3 1
#> 
#> $features[[3]]
#> [1] 1 1
#> 
#> $features[[4]]
#> [1] 2 1
#> 
#> 
#> $type
#> [1] "R"
```

## Train Structured RerF (S-RerF) for image classification:

S-RerF samples and evaluates a set of random features at each split
node, where each feature is defined as a random linear combination of
intensities of pixels contained in a contiguous patch within an image.
Thus, the generated features exploit local structure inherent in images.

To be able to run this example quickly we will consider training and
testing on the digits `3` and `5`. You can try a differernt subset of
digits by changing `numsub` in the code chunk below.

``` r
data(mnist)

## Get a random subsample, 100 each of 3's and 5's
set.seed(320)
threes <- sample(which(mnist$Ytrain %in% 3), 100)
fives  <- sample(which(mnist$Ytrain %in% 5), 100)
numsub <- c(threes, fives)

Ytrain <- mnist$Ytrain[numsub]
Xtrain <- mnist$Xtrain[numsub,]
Ytest <- mnist$Ytest[mnist$Ytest %in% c(3,5)]
Xtest <- mnist$Xtest[mnist$Ytest %in% c(3,5),]

# p is number of dimensions, d is the number of random features to evaluate, iw is image width, ih is image height, patch.min is min width of square patch to sample pixels from, and patch.max is the max width of square patch
p <- ncol(Xtrain)
d <- ceiling(sqrt(p))
iw <- sqrt(p)
ih <- iw
patch.min <- 1L
patch.max <- 5L
forest <- RerF(Xtrain, Ytrain, num.cores = 1L, FUN = RandMatImagePatch,
               paramList = list(p = p, d = d, iw = iw, ih = ih,
                                pwMin = patch.min, pwMax = patch.max),
               seed = 1L)
predictions <- Predict(Xtest, forest, num.cores = 1L)
mnist.error.rate <- mean(predictions != Ytest)
```

**Expected output**

``` r
mnist.error.rate
#> [1] 0.0362776
```

## Unsupervised classification (U-RerF)

Using the Iris dataset we will show how to use the unsupervised verison.

``` r
set.seed(54321)
X <- as.matrix(iris[, 1:4])

u1 <- Urerf(X, trees = 100, Progress = FALSE)
```

### The dissimilarity matrix

``` r
m <- as.matrix(u1$sim)
#plot(as.raster(m))
```

### Running h-clust on the resulting dissimiliarity matrix

``` r
disSim <- hclust(as.dist(1 - u1$similarityMatrix), method = 'mcquitty')
clusters <- cutree(disSim, k = 3)
table(clusters, truth = as.numeric(iris[[5]]))
#>         truth
#> clusters  1  2  3
#>        1 50  0  0
#>        2  0 20 37
#>        3  0 30 13
```

<!-- calcium-spike data are not properly documented at this time, waiting on @jasonkyuyim TBD by 20180813 -->

<!--
### Train Structured RerF (S-RerF) for spike train inference:

Similar to S-RerF for image classification except now in the Spike Train setting. 500 samples were stimulated from the following AR(2) model:

$$c_t = \sum_{i}{\gamma_i c_{t-i} + s_t},\quad  s_t \sim \text{Poisson}(0.01)$$

$$y_t = a \ c_t + \epsilon_t, \ \ \ \ \epsilon_t \sim \mathcal{N}(0, 1)$$


where $\gamma_1 = 1.7, \gamma_2 = -0.712$, $a = 1$. We sampled such that the were an equal number of spikes and non-spikes in the datasets. S-RerF was trained on these samples by computing local feature patches across the time series windows.



**Expected output**


-->

-----
