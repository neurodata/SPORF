# R-RerF
## This is the R implementation of Randomer Forest([RerF](https://arxiv.org/pdf/1506.03410v2.pdf "arxiv link to RerF paper"))


## Features of R-Rerf:
- [x] Train a forest
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
   3. In R --> ```forest <- rfr(X, Y, MinParent=6, trees=100, MaxDepth=0, bagging = .20, FUN=makeA, options=ncol(X), COOB=FALSE, Progress=FALSE)```  
  - **forest** --> this is the return value from rfr.  the trained forest structure is used as input to the other functions.
  - **X** --> an n by d dataset where the rows are the n samples and the columns are the p features.  Can be a matrix or dataframe.
  - **Y** --> a vector of n class labels.  Labels must start at 1 and rise sequentially (e.g. if you have three classes then the first one is '1', then second one is '2', and the third one is '3'.
  - **MinParent** --> an integer greater than 1.  This defines the minimum node size.  Any node with MinParent elements will not have a child node.  The default value is 6 (MinParent=6).
  - **Trees** --> an integer greater than 0.  This is the number of trees that will be in the forest.  The default value is 100 (trees=100).
  - **MaxDepth** --> is the maximum depth that a tree can grow to.  If set to "inf" then there is no maximum depth.  If set to 0 then a maximum depth is calculated based on the number of classes and number of samples provided.  The default value is 0 (MaxDepth=0).
  - **bagging** --> is the percentage of training data to withhold during each training iteration.  If set to 0 then the entire training set is used during every iteration.  The withheld portion of the training data  is used to calculate OOB error for the tree.  The default is .2, so 80% of the training set is used to train a tree and the other 20% will be used for cross validation (bagging=.2).
  - **FUN** --> is the function used to create the projection matrix.  The matrix returned by this function should be a p-by-u matrix where p is the number of columns in the input matrix X and u is any integer > 0.  u can also vary from node to node.  The default is makeA, a description of which can be found below.
  - **options** --> is a list of inputs to the user provided projection matrix creation function -- FUN.  Option is set to ncol(X) by default (options=ncol(X)).
  - **COOB** --> when TRUE, OOB error is calculated and statistics are recorded for each tree: which samples were OOB and the tree calculated liklihood of each OOB sample being in each class.  COOB is set to FALSE by default (COOB=FALSE).
  - **Progress** --> when TRUE, a progress bar is printed after each tree is trained.  This can be useful when training a forest on a large dataset.  Progress is set to FALSE by default (Progress=FALSE).
  
###   To determine the error rate of a forest:
   1. Download rfr_function.R
   2. In R --> ```source('path/to/file/rfr_function.R')```
   3. In R --> ```Error_Rate <- error_rate(X,Y,forest)```
  - **Error_Rate** --> this is the return value for error_rate.  1 >= Error_Rate >= 0.
  - **X** --> an n by d dataset where the rows are the n samples and the columns are the p features.  Can be a matrix or dataframe.
  - **Y** --> a vector of n class labels.  Labels must start at 1 and rise sequentially (e.g. if you have three classes then the first one is '1', then second one is '2', and the third one is '3').
  - **forest** --> This is a type created by the rfr training function.

###   To make a prediction on data:
   1. Download rfr_function.R
   2. In R --> ```source('path/to/file/rfr_function.R')```
   3. In R --> ```Predictions <- predict(X,forest)```
  - **Predictions** --> this is the return value of predict.  Predictions is a vector of length n, the number of samples in X.  Each element in the vector is the prediction of the corresponding sample in X.
  - **X** --> an n by d dataset where the rows are the n samples and the columns are the p features.
  - **forest** --> This is a type created by the rfr training function.
  
###   To see how OOB error with the number of trees:  
   1. Download rfr_function.R  
   2. In R --> ```source('path/to/file/rfr_function.R')```  
   3. In R --> ```OOBg <- OOBgrow(Y,forest)```  
   - **OOBg** --> this is the return value of OOBgrow.  OOBg is a vector with a length equal to the number of trees in the forest.  Each element in OOBg is the OOB error if the forest consisted of the element's index number of trees (e.g. the fourth element in OOBg is the OOB error for the first four trees; the tenth element in OOBg is the OOB error for the first ten trees).
   - **Y** --> a vector of n class labels.  Labels must start at 1 and rise sequentially (e.g. if you have three classes then the first one is '1', then second one is '2', and the third one is '3').  This must be the same Y used to train the forest.
   - **forest** --> This is a type created by the rfr training function.  When training the forest the bagging options must be greater than zero and COOB must be TRUE or OOBgrow will not work.

## Example using iris data set:
```R
source('path/to/file/rfr_function.R')

X <- as.matrix(iris[,1:4])  
Y <- as.numeric(iris[,5])  
# basic use of forest generator
forest <- rfr(X,Y, bagging=.2, COOB=TRUE)  
error_rate(X,Y,forest)  
predict(X,forest)
OOBgrow(Y,forest)

# basic use of projection matrix creation. A non-toy example of a
# projection matrix can be found at the end of this document.  The 
# projection matrix created below is the identity matrix which is 
# then converted to a sparse format.  The trees grown using this 
# projection matrix will be identical; each one will be a traditional 
# decision tree.  The last two lines in this function should be in 
# every projection matrix creation function in order to convert the 
# matrix into a sparse format.  
ID <- function(options){
   sparseM<-diag(options)
   ind<- which(sparseM!=0,arr.ind=TRUE)
   return(cbind(ind,sparseM[ind]))
}

forest2 <- rfr(X,Y,FUN=ID)
error_rate(X,Y,forest2)
predict(X,forest2)

# advanced use of projection matrix creation.  The projection matrix 
# is similar to the one above but this time it is scaled by half.    
IDscale <- function(options){
   size <- options[1]
   scale <- options[2]
   sparseM<-diag(size)*scale
   ind<- which(sparseM!=0,arr.ind=TRUE)
   return(cbind(ind,sparseM[ind]))
}

forest3 <- rfr(X, Y, MinParent=10, trees=1000, MaxDepth=6, bagging = .10, FUN=IDscale, options=c(ncol(X), .5))
error_rate(X,Y,forest3)
predict(X,forest3)
```
Additional examples can be found in the .rmd files located in the "versions" directory

## Default projection matrix creation:
This is the function that rfr uses by default to create a projection matrix.
```R
makeA <- function(options){
    p <- options # by default this is the number of features in X (i.e. the number of columns in X).
    mtry <- ceiling(log2(p)) # this determines the number of columns in the projection matrix.
    #Create the A matrix, a sparse matrix of 1's, -1's, and 0's.
    sparseM <- matrix(0L,nrow=p,ncol=mtry)
    sparseM[sample(1:(p*mtry),mtry,replace=F)] <- 1L
    sparseM[sample(1:(p*mtry),mtry,replace=F)] <- -1L
    #The below returns a matrix after removing zero columns in sparseM.
    #These two lines should be used to convert the projection matrix into a sparse matrix.
    ind<- which(sparseM!=0,arr.ind=TRUE)
    return(cbind(ind,sparseM[ind]))
}
```
