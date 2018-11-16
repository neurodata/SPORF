Changes in 2.0.0:

* The name of the algorithm has changed from `Randomer Forest` to
  `LumberJack`. As such the project code is now hosted at
  [neurodata/lumberjack](https://github.com/neurodata/lumberjack).  In
  future updates there may be some changes to function names, which will
  be noted in the NEWS file.

* The option `rho` in the RerF function has been re-named to `sparsity`
  to match with the algorithm explanation 

* Predictions are made based on the average of posteriors rather than average of the predictions. 

* The included RandMat functions have been re-structured for ease of use with their
  own examples and documentation.  This should make it easier to create
  and include a user defined function to use as an input option.

* We are now using `testthat` for all of our function tests moving
  forward. 

* Housekeeping: Updated the README and changed maintainers.

