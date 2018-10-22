context("Regression housing prices and OOB Predictions")
library(rerf)
library(MASS)

set.seed(123456)

# Get boston housing
X <- as.matrix(Boston[, 1:13])
Y <- Boston[[14L]]

# # Set training and testing data
# trainIdx <- c(1:30, 51:80, 101:120)
# X.train <- X[trainIdx, ]
# Y.train <- Y[trainIdx]
# X.test <- X[-trainIdx, ]
# Y.test <- Y[-trainIdx]

# test_that("Invalid task raises error", {
#   forest <- RerF(X, Y, seed = 1L, num.cores = 1L, store.oob = FALSE)
#   oob.predictions <- expect_error(OOBPredict(X, forest))
# })

