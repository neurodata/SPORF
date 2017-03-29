# R-RerF
## This is the R implementation of Randomer Forest([RerF](https://arxiv.org/pdf/1506.03410v2.pdf "arxiv link to RerF paper"))


## Features of R-Rerf:
- [x] Train a forest on a given dataset
- [x] Determine Error Rate of Forest
- [x] Make Predictions
### Future Improvements:
- Bagging
- Out-Of-Bag Error Estimation
- Max Depth Limit

## Use:
###   To create a forest:
 1. Download rfr_function.R
 2. In R --> source('path/to/file/rfr_function.R')
 3. In R --> **forest** <- rfr(**X**,**Y**,**MTRY**,**MinParent**,**Trees**)
  - **forest** --> the trained forest structure is used as input in the error_rate and predict functions.
  - **X** --> an n by d dataset where the rows are the n samples and the columns are the p features.
  - **Y** --> a vector of n class labels.  Labels must start at 1 and rise sequentially (e.g. if you have three classes then the first one is '1', then second one is '2', and the third one is '3'.
  - **MTRY** --> an integer less than or equal to p.  This is the number of random features to evaluate at each node.  If MTRY equals p then all features are considered at each node which is equivalent to making a decision tree.  If mtry=0 then mtry is set to the ceiling of the square root of p.  The default value of mtry is 0 (mtry=0).
  - **MinParent** --> an integer greater than 1.  This defines the minimum node size.  Any node with MinParent elements will not have a child node.  The default value is 6 (MinParent=6).
  - **Trees** --> an integer greater than 0.  This is the number of trees that will be in the forest.  The default value is 100 (trees=100).

###   To determine the error rate of a forest:
 1. Download rfr_function.R
 2. In R --> source('path/to/file/rfr_function.R')
 3. In R --> Error_Rate <- error_rate(**X**,**Y**,**Forest**)
  - **X** --> an n by d dataset where the rows are the n samples and the columns are the p features.
  - **Y** --> a vector of n class labels.  Labels must start at 1 and rise sequentially (e.g. if you have three classes then the first one is '1', then second one is '2', and the third one is '3'.
  - **Forest** --> This is a type created by the rfr training function.

###   To make a prediction on data:
   1. Download rfr_function.R
   2. In R --> source('path/to/file/rfr_function.R')
   3. In R --> Predictions <- predict(**X**,**Forest**)
    - **X** --> an n by d dataset where the rows are the n samples and the columns are the p features.
    - **Forest** --> This is a type created by the rfr training function.
