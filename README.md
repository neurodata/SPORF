# R-RerF
## This is the R implementation of Randomer Forest.

## Features of R-Rerf:
- [x] Train a tree on a given dataset
- [x] Train a forest on a given dataset
- [x] Determine Error Rate of Forest
- [ ] Make Predictions

## Use:
###   To build a tree:
 1. Download rfr_function.R
 2. In R --> source('path/to/file/rfr_function.R')
 3. In R --> tree_name <- build_tree(**X**,**Y**,**MTRY**,**Max_depth**)
  - **X** --> an n by d dataset where the rows are the n samples and the columns are the p features.
  - **Y** --> a vector of n class labels.  Labels must start at 1 and rise sequentially (e.g. if you have three classes then the first one is '1', then second one is '2', and the third one is '3'.
  - **MTRY** --> a scalar less than or equal to p.  This is the number of random features to evaluate at each node.  If MTRY equals p then all features are considered at each node which is equivalent to making a decision tree.
  - **Max_depth** --> a scalar greater than 1.  This defines the maximum depth of each tree.

###   To create a forest:
 1. Download rfr_function.R
 2. In R --> source('path/to/file/rfr_function.R')
 3. In R --> forest_name <- rfr(**X**,**Y**,**MTRY**,**Max_depth**,**Number of
	Trees**)
  - **X** --> an n by d dataset where the rows are the n samples and the columns are the p features.
  - **Y** --> a vector of n class labels.  Labels must start at 1 and rise sequentially (e.g. if you have three classes then the first one is '1', then second one is '2', and the third one is '3'.
  - **MTRY** --> a scalar less than or equal to p.  This is the number of random features to evaluate at each node.  If MTRY equals p then all features are considered at each node which is equivalent to making a decision tree.  The default value is the ceiling of the square root of p.
  - **Max_depth** --> a scalar greater than 1.  This defines the maximum depth of each tree.  The default value is 6.
  - **Number of Trees** --> a scalar greater than 0.  This is the number of
    trees that will be in the forest.  The default is 100.

###   To determine the error rate of a forest:
 1. Download rfr_function.R
 2. In R --> source('path/to/file/rfr_function.R')
 3. In R --> Error_Rate <- error_rate(**X**,**Y**,**Forest**)
  - **X** --> an n by d dataset where the rows are the n samples and the columns are the p features.
  - **Y** --> a vector of n class labels.  Labels must start at 1 and rise sequentially (e.g. if you have three classes then the first one is '1', then second one is '2', and the third one is '3'.
  - **Forest** --> This is a type created by the rfr training function.

###   To make a prediction on data:
   - Not yet implemented.
