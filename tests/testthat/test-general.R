context("General tests")
## These tests were originally in travisTest/test.R and this is an attempt to port
## them to test_that

library(rerf)

# Get iris
X <- as.matrix(iris[, 1:4])
Y <- iris[[5L]]

# Set training and testing data
trainIdx <- c(1:30, 51:80, 101:120)
X.train <- X[trainIdx, ]
Y.train <- Y[trainIdx]
X.test <- X[-trainIdx, ]
Y.test <- Y[-trainIdx]

test_that("Testing number of trees output is as requested.", {
  numTrees = c(1, 10, 50, 500)
  for (nn in numTrees) {
    forest <- RerF(X, Y, trees = nn, seed = 1L, num.cores = 1L, store.oob = FALSE)
    expect_equal(length(forest$trees), nn)
  }
})

# sum of the class probabilities should ~= 1 alternatively -- every leaf node
# should be > 0, probability be > 0, or sum ~= 1
test_that("Sum of the class probabilities should ~= 1.", {
  numTrees = 10
  forest <- RerF(X, Y, trees = numTrees, seed = 1L, num.cores = 1L, store.oob = FALSE)
  for (z in 1:length(forest$trees)) {
    for (q in 1:length(forest$trees[[z]]$ClassProb[, 1])) {
      expect_equal(sum(forest$trees[[z]]$ClassProb[q, ]), 1, tolerance = 1e-08)
    }
  }
})
