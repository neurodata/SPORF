context("Feature Importance")
library(rerf)

# Get iris
X <- as.matrix(iris[, 1:4])
Y <- iris[[5L]]

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
