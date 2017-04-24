# R-RerF
## This is the R implementation of Randomer Forest([RerF](https://arxiv.org/pdf/1506.03410v2.pdf "arxiv link to RerF paper"))


## Features of R-Rerf:
- [x] Train a forest on a given dataset
- [x] Determine Error Rate of Forest
- [x] Make Predictions
- [x] Bagging
- [x] Out-Of-Bag Error Estimation
- [x] Max Depth Limit
- [x] Projection matrix as parameter

## Use:
###   To create a forest:
   1. Download rfr_function.R
   2. In R --> ```source('path/to/file/rfr_function.R')```  
   3. In R --> ```forest <- rfr(X, Y, MinParent=6, trees=100, MaxDepth=0, bagging = .20, FUN=makeA, options=ncol(X))```  
  - **forest** --> the trained forest structure is used as input in the error_rate and predict functions.
  - **X** --> an n by d dataset where the rows are the n samples and the columns are the p features.
  - **Y** --> a vector of n class labels.  Labels must start at 1 and rise sequentially (e.g. if you have three classes then the first one is '1', then second one is '2', and the third one is '3'.
  - **MinParent** --> an integer greater than 1.  This defines the minimum node size.  Any node with MinParent elements will not have a child node.  The default value is 6 (MinParent=6).
  - **Trees** --> an integer greater than 0.  This is the number of trees that will be in the forest.  The default value is 100 (trees=100).
  - **MaxDepth** --> is the maximum depth that a tree can grow to.  If set to "inf" then there is no maximum depth.  If set to 0 then a maximum depth is calculated based on the number of classes and number of samples provided.  The default value is 0 (MaxDepth=0).
  - **bagging** --> is the percentage of training data to withhold during each training iteration.  If set to 0 then the entire training set is used during every iteration.  The withheld portion of the training data  is used to calculate OOB error for the tree.  The default is .2, so 80% of the training set is used to train a tree and the other 20% will be used for cross validation (bagging=.2).
  - **FUN** --> is the function used to create the projection matrix.  The matrix returned by this function should be a p-by-u matrix where p is the number of columns in the input matrix X and u is any integer > 0.  u can also vary from node to node.  The default is makeA, a description of which can be found below.
  - **options** --> is a list of inputs to the user provided projection matrix creation function -- FUN.  Option is set to ncol(X) by default (options=ncol(X)).
###   To determine the error rate of a forest:
   1. Download rfr_function.R
   2. In R --> ```source('path/to/file/rfr_function.R')```
   3. In R --> ```Error_Rate <- error_rate(X,Y,forest)```
  - **X** --> an n by d dataset where the rows are the n samples and the columns are the p features.
  - **Y** --> a vector of n class labels.  Labels must start at 1 and rise sequentially (e.g. if you have three classes then the first one is '1', then second one is '2', and the third one is '3'.
  - **forest** --> This is a type created by the rfr training function.

###   To make a prediction on data:
   1. Download rfr_function.R
   2. In R --> ```source('path/to/file/rfr_function.R')```
   3. In R --> ```Predictions <- predict(X,forest)```
  - **X** --> an n by d dataset where the rows are the n samples and the columns are the p features.
  - **forest** --> This is a type created by the rfr training function.

## Example using iris data set:
```R
source('path/to/file/rfr_function.R')
# The use of the following five lines is optional, but will cut the time 
# required for training and testing by 2/3s.
library(compiler)
setCompilerOptions("optimize"=3)
rfr <- cmpfun(rfr)
error_rate <- cmpfun(error_rate)
predict <- cmpfun(predict)

X <- as.matrix(iris[,1:4])  
Y <- as.numeric(iris[,5])  
# basic use of forest generator
forest <- rfr(X,Y)  
error_rate(X,Y,forest)  
predict(X,forest)

# basic use of function creation
ID <- function(options){
diag(options)}

forest2 <- rfr(X,Y,FUN=ID)
error_rate(X,Y,forest2)
predict(X,forest2)

# advanced use of forest generator
IDscale <- function(options){
size <- options[1]
scale <- options[2]
diag(size)*scale}

forest3 <- rfr(X, Y, MinParent=10, trees=1000, MaxDepth=6, bagging = .10, FUN=IDscale, options=c(ncol(X), .5))
error_rate(X,Y,forest3)
predict(X,forest3)
```
Additional examples can be found in the .rmd files located in the "versions" directory

## Default projection matrix creation:
This is the function that rfr uses by default to create a projection matrix.
```R
makeA <- function(options){
 p <- options
 mtry <- ceiling(log2(p))
 # Create the A matrix, a sparse matrix of 1's, -1's, and 0's.
 sparseM <- matrix(0,nrow=p,ncol=mtry)
 sparseM[sample(1:(p*mtry),mtry,replace=F)] <- 1
 sparseM[sample(1:(p*mtry),mtry,replace=F)] <- -1
 #The below returns a matrix after removing all columns that only contain zeroes.
 sparseM <- as.matrix(sparseM[,!apply(sparseM==0,2,all)])
}
```
