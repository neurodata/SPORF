context("Urerf")
library(rerf)

set.seed(123456)


test_that("bestCutForFeature", {
  combinedFeature <- rep(5, 20)

  expect_true(is.null(TwoMeansCut(combinedFeature)))

  a <- 5

  b <- 10

  combinedFeature <- c(rep(a, 20), rep(b, 20))

  expect_equal(TwoMeansCut(combinedFeature), c((a + b) / 2, 0))

  combinedFeature <- c(runif(10), runif(20) + 2)

  result <- TwoMeansCut(combinedFeature)

  expect_equal(result[1], mean(sort(combinedFeature)[10:11]))

  expect_true(result[2] > 0)

  a <- 0

  b <- 10

  combinedFeature <- c(rep(a, 20), rep(b, 20))

  expect_equal(TwoMeansCut(combinedFeature), c((a + b) / 2, 0))
})





test_that("matrix attributes are legal", {
  X <- NULL

  expect_error(checkInputMatrix(X), "the input is null.")

  X <- cbind(0, matrix(runif(25), nrow = 5))

  expect_error(checkInputMatrix(X), "some columns are all zero.")

  X <- matrix(runif(25), nrow = 5)

  X[20] <- NA

  expect_error(checkInputMatrix(X), "some values are na or nan.")

  X[20] <- NaN

  expect_error(checkInputMatrix(X), "some values are na or nan.")
})


test_that("output is the right size and type", {
  X <- as.matrix(iris[, 1:4])

  similarityMatrix <- Urerf(X, 100, 10, Progress = FALSE)$similarityMatrix


  expect_equal(nrow(similarityMatrix), 150)

  expect_equal(ncol(similarityMatrix), 150)
})
