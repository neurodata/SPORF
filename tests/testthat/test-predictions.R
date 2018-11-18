context("Predictions and OOB Predictions")
library(rerf)

set.seed(123456)

# Get iris
X <- as.matrix(iris[, 1:4])
Y <- iris[[5L]]

# Set training and testing data
trainIdx <- c(1:30, 51:80, 101:120)
X.train <- X[trainIdx, ]
Y.train <- Y[trainIdx]
X.test <- X[-trainIdx, ]
Y.test <- Y[-trainIdx]

test_that("OOB predictions fails when OOB indices not stored", {
  forest <- RerF(X, Y, seed = 1L, num.cores = 1L, store.oob = FALSE)
  oob.predictions <- expect_error(OOBPredict(X, forest))
})

test_that("Predictions fail when training data not given for rank.transform", 
  {
    forest <- RerF(X, Y, seed = 1L, num.cores = 1L, store.oob = TRUE, 
      min.parent = 1, max.depth = 0, rank.transform = TRUE)
    expect_error(oob.predictions <- OOBPredict(X, forest))
  })

test_that("Test fails when input is not matrix", {
  forest <- RerF(X, Y, seed = 1L, num.cores = 1L, store.oob = TRUE, min.parent = 1, 
    max.depth = 0, rank.transform = TRUE)
  bad.input <- "bad"
  expect_error(OOBPredict(bad.input, forest))
})

test_that("Iris OOB Predictions", {
  # Build as large of trees as possible
  forest <- RerF(X, Y, seed = 1L, num.cores = 1L, store.oob = TRUE, min.parent = 1, 
    max.depth = 0)
  oob.predictions <- OOBPredict(X, forest, num.cores = 1L)
  accuracy <- mean(Y == oob.predictions)
  expect_equal(accuracy, 144/150)
  
  # Limit depth of trees
  forest <- RerF(X, Y, seed = 1L, num.cores = 1L, store.oob = TRUE, min.parent = 1, 
    max.depth = 2)
  oob.predictions <- OOBPredict(X, forest, num.cores = 2L)
  accuracy <- mean(Y == oob.predictions)
  expect_equal(accuracy, 139/150)
})

test_that("Iris Predictions", {
  # Build as large of trees as possible
  forest <- RerF(X.train, Y.train, seed = 3L, num.cores = 1L, min.parent = 1, 
    max.depth = 0)
  predictions <- Predict(X.test, forest, num.cores = 1L)
  accuracy <- mean(Y.test == predictions)
  expect_equal(accuracy, 65/70)
  
  # Limit depth of trees
  forest <- RerF(X.train, Y.train, seed = 3L, num.cores = 1L, min.parent = 1, 
    max.depth = 3L)
  predictions <- Predict(X.test, forest, num.cores = 1L)
  accuracy <- mean(Y.test == predictions)
  expect_equal(accuracy, 64/70)
})

test_that("Output probabilities should equal 1", {
  forest <- RerF(X.train, Y.train, seed = 4L, num.cores = 1L, store.oob = TRUE, 
    min.parent = 1, max.depth = 3)
  # Test data predictions
  predictions <- Predict(X.test, forest, output.scores = TRUE, num.cores = 1L)
  nrows <- nrow(predictions)
  expect_equal(rep(1, nrows), rowSums(predictions))
  
  # OOB data predictions
  oob.predictions <- OOBPredict(X.train, forest, output.scores = TRUE, num.cores = 2L)
  nrows <- nrow(oob.predictions)
  expect_equal(rep(1, nrows), rowSums(oob.predictions))
})

test_that("Not aggregate output, probabilities should still equal 1", {
  forest <- RerF(X.train, Y.train, seed = 5L, num.cores = 1L, store.oob = TRUE, 
    min.parent = 1, max.depth = 3)
  
  predictions <- Predict(X.test, forest, output.scores = TRUE, aggregate.output = FALSE, num.cores = 2L)
  trees <- length(predictions)
  for (i in 1:trees) {
    tree.predictions <- predictions[[i]]
    nrows <- nrow(tree.predictions)
    expect_equal(rep(1, nrows), rowSums(tree.predictions))
  }
})
