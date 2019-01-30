## Changes in 2.0.3:

* The `PrintTree` function has been added to aid in viewing the
  cutpoints, features, and other statistics in a particular tree of a
  forest.

* Urerf now supports using the Bayesian information criterion (BIC) from
  the `mclust` package for determining the best split.

* Feature importance calculations now correctly handle features whose
  weight vectors parametrize the same line.  Also, when the projection
  weights are continuous we tabulate how many times a unique combination
  of features was used, ignoring the weights.

* The base functionality is being moved into C++.


## Changes in 2.0.2:

* The option `rho` in the RerF function has been re-named to `sparsity`
  to match with the algorithm explanation.

* The default parameters sent to the RandMat\* functions now properly
  account for categorical columns.

* The defaults have changed for the following parameters:
  * `min.parent = 1`
  * `max.depth  = 0`
  * `stratify   = TRUE`

* Predictions are made based on the average of posteriors rather than average of the predictions. 

* The included RandMat\* functions have been re-structured for ease of use with their
  own examples and documentation.  This should make it easier to create
  and include a user defined function to use as an input option.

* We are now using `testthat` for all of our function tests moving
  forward. 

* Housekeeping: Updated the README and changed maintainers.

