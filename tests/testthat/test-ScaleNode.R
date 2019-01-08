context("Scale* tests")
library(rerf)

test_that("Testing Scale01 for d=1", {
  # Create a random data matrix to be scaled
  X <- runif(100)

  sdat <- Scale01(X)

  r <- apply(sdat$scaledXnode, 2, range)

  ## Check that ranges specified are in [0,1]
  expect_true(all(r[1, ] == 0))
  expect_true(all(r[2, ] == 1))

  ## Check that min and diff are of correct lenghts
  expect_true(length(sdat$scalingFactors$min) == 1)
  expect_true(length(sdat$scalingFactors$diff) == 1)
})

test_that("Testing Scale01 for d=2", {
  # Create a random data matrix to be scaled
  X <- matrix(runif(100), 50, 2)

  sdat <- Scale01(X)

  r <- apply(sdat$scaledXnode, 2, range)

  ## Check that ranges specified are in [0,1]
  expect_true(all(r[1, ] == 0))
  expect_true(all(r[2, ] == 1))

  ## Check that min and diff are of correct lenghts
  expect_true(length(sdat$scalingFactors$min) == ncol(X))
  expect_true(length(sdat$scalingFactors$diff) == ncol(X))
})
