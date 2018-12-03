Changes in 2.0.0:

* The option `rho` in the RerF function has been re-named to `sparsity`
  to match with the algorithm explanation.

* The default parameters sent to the RandMat\* functions now properly
  account for categorical columns.

* The defualts have changed for the following parameters:
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

