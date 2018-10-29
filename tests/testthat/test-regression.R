context("Regression housing prices and OOB Predictions")
library(rerf)
library(MASS)
library(randomForest)

set.seed(123456)

## Get boston housing
# d <- dim(Boston)[2] - 1
# N <- dim(Boston)[1]
# X <- Boston[, 1:d]
# Y <- Boston[[14L]]
# train_split <- 0.8
# test_split <- 0.2
# train_indices <- sample(1:N, N * train_split, replace = FALSE)
# X.train <- as.matrix(X[train_indices, ])
# Y.train <- Y[train_indices]
# X.test <- as.matrix(X[-train_indices, ])
# Y.test <- Y[-train_indices]

d <- 3
N <- 6
X <- matrix(
  c(0,1,0,
    0,1,0,
    0,0,1,
    0,0,1,
    0,1,1,
    0,1,1),
  nrow=N,
  ncol=d,
  byrow=TRUE)
Y <- c(2,2,4,4,8,8)
X.train <- as.matrix(X)
Y.train <- Y
X.test <- as.matrix(X)
Y.test <- Y

# X.test <- as.matrix(X[train_indices, ])
# Y.test <- Y[train_indices]

rerf.forest <- RerF(
  X.train,
  Y.train,
  seed = 3L,
  trees = 1L,
  FUN = RandMatRF,
  paramList = list(p = ncol(X.train),
                   d = ncol(X.train)),
  num.cores = 1L,
  task = 'classification',
  min.parent=6,
  bagging=1L
)
rerf_predictions <- Predict(X.test, rerf.forest, task = 'regression', num.cores=1L)
rerf_accuracy <- mean((rerf_predictions - Y.test) ^ 2)
rerf_accuracy

rf.forest <- randomForest(
  x = X.train,
  y = Y.train,
  xtest = X.test,
  ytest = Y.test,
  mtry = d,
  ntree = 1,
  keep.forest=TRUE
)
rf_predictions <- rf.forest$test$predicted
rf_accuracy <- mean((rf_predictions - Y.test) ^ 2)
rf_accuracy

test_that("Boston Housing Predictions", {
  # Build as large of trees as possible
  forest <- RerF(X[train_indices,], Y[train_indices,], seed = 3L, num.cores = 1L,
                 min.parent = 1,
                 max.depth = 0,
                 tas='regression')
})

# test_that("Invalid task raises error", {
#   forest <- RerF(X, Y, seed = 1L, num.cores = 1L, store.oob = FALSE)
#   oob.predictions <- expect_error(OOBPredict(X, forest))
# })

