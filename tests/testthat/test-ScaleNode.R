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



test_that("Testing a scaled forest", {
  # Create a random data matrix to be scaled
  train.idx <- sample(nrow(iris), 125)

  X <- iris[train.idx, -5]
  Y <- iris[[5]][train.idx]

  xt <- iris[-train.idx, -5]
  yt <- iris[[5]][-train.idx]
  forest <- RerF(X, Y, FUN = RandMatBinary, paramList = list(p = 4, d = 1, sparsity = 0.2, prob = 0.1),
                 scaleAtNode = TRUE, num.cores = 1L)
  Yhat <- Predict(X, forest)
  expect_equal(Yhat, Y)
})
