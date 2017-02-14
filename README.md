# R-RerF
## This is the R implementation of Randomer Forest.

## Features of R-Rerf:
- [ ] Train a forest on a given dataset
- [x] Train a tree on a given dataset
- [ ] Determine Error Rate of Forest
- [ ] Make Predictions

## Use:
###   To build a tree:
 1. Download rf_function.R  
 2. In R --> source('path/to/file/rf_function.R')  
 3. In R --> build_tree(**X**,**Y**,**MTRY**,**Max_depth**)  
  - **X** --> an n by d dataset where the rows are the n samples and the columns are the p features.
  - **Y** --> a vector of n class labels.  Labels must start at 0 and rise sequentially (e.g. if you have three classes then the first one is '0', then second one is '1', and the third one is '2'.
  - **MTRY** --> a scalar less than or equal to p.  This is the number of random features to evaluate at each node.  If MTRY equals p then all features are considered at each node which is equivalent to making a decision tree.
  - **Max_depth** --> a scalar greater than 1.  This defines the maximum depth of each tree.

###   To create a forest:  
   - Not yet implemented.
  
###   To determine the error rate of a forest:  
   - Not yet implemented.
  
###   To make a prediction on data:  
   - Not yet implemented.  
