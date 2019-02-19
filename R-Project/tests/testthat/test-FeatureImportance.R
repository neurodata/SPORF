context("Feature Importance")
library(rerf)

# Get iris
X <- as.matrix(iris[, 1:4])
Y <- iris[[5L]]

test_that("Test that the correct features are removed/kept", {

  ## Create synthetic feature/weight list:
  uni <-
    unique(
      list(
        c(1L, 1L),
        c(2L, 1L),
        c(3L, 1L),
        c(4L, 1L),
        #
        c(1L, -1L),
        c(2L, -1L),
        c(3L, -1L),
        c(4L, -1L),
        #
        c(1L, 1L, 2L, 1L),
        c(1L, 1L, 3L, 1L),
        c(1L, 1L, 4L, 1L),
        c(2L, 1L, 3L, 1L),
        c(2L, 1L, 4L, 1L),
        c(3L, 1L, 4L, 1L),
        #
        c(1L, -1L, 2L, 1L),
        c(1L, -1L, 3L, 1L),
        c(1L, -1L, 4L, 1L),
        c(2L, -1L, 3L, 1L),
        c(2L, -1L, 4L, 1L),
        c(3L, -1L, 4L, 1L),
        #
        c(1L, 1L, 2L, -1L),
        c(1L, 1L, 3L, -1L),
        c(1L, 1L, 4L, -1L),
        c(2L, 1L, 3L, -1L),
        c(2L, 1L, 4L, -1L),
        c(3L, 1L, 4L, -1L),
        #
        c(1L, -1L, 2L, -1L),
        c(1L, -1L, 3L, -1L),
        c(1L, -1L, 4L, -1L),
        c(2L, -1L, 3L, -1L),
        c(2L, -1L, 4L, -1L),
        c(3L, -1L, 4L, -1L)
      )
    )


  p <- 4 ## p would normally come from the forest object
  uni.equiv <- rerf:::uniqueByEquivalenceClass(p, uni)

  expect_equal(length(which(uni %in% uni.equiv)), 16)

  ## get indices of projections removed from full list
  up.ind <- which(!(uni %in% uni.equiv))

  ## Check that all of the flips of unique.projections.equiv are in
  expect_true(
    all(
      lapply(uni[up.ind], rerf:::flipWeights) %in% uni.equiv
    )
  )
})




test_that("Test equivalent sum of scores between methods", {

  # This checks to make sure the sums of scores of a tree
  # are equivalent
  forest <- RerF(X, Y, trees = 1L, seed = 1L, num.cores = 1L, store.impurity = TRUE)

  tree <- forest$trees[[1]]
  num.trees <- length(forest$trees)
  num.splits <- sapply(forest$trees, function(tree) length(tree$CutPoint))

  unique.projections <- vector("list", sum(num.splits))

  idx.start <- 1L

  for (t in 1:num.trees) {
    idx.end <- idx.start + num.splits[t] - 1L
    unique.projections[idx.start:idx.end] <- lapply(1:num.splits[t], function(nd) forest$trees[[t]]$matAstore[(forest$trees[[t]]$matAindex[nd] + 1L):forest$trees[[t]]$matAindex[nd + 1L]])
    idx.start <- idx.end + 1L
  }

  unique.projections <- unique(unique.projections)
  unique.projections.equiv <- rerf:::uniqueByEquivalenceClass(forest$params$paramList$p, unique(unique.projections))

  ## The two different methods should return the same sum of
  ## scores.  The only change is which scores gets assigned to which
  ## feature.
  a <- sum(rerf:::RunFeatureImportance(tree, unique.projections))
  b <- sum(rerf:::RunFeatureImportanceBinary(tree, unique.projections.equiv))

  expect_equal(a, b)
})
