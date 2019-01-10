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


test_that("Testing cut values of a scaled forest", {
  ## The dataset could be something else here
  X <- iris[, -5]
  Y <- iris[[5]]

  Ntrees <- 3L

  ## Generate a scaled forest of 3 trees
  forest <- RerF(X, Y,
    FUN = RandMatBinary,
    paramList = list(p = 4, d = 3, sparsity = 0.25, prob = 0.1),
    scaleAtNode = TRUE, num.cores = 1L, trees = Ntrees
  )

  cutF <- vector("list", Ntrees)
  cutV <- vector("list", Ntrees)

  ## for each tree get cut points and features used to split with their
  ## weights and store them
  for (numTree in 1:2) {
    CutValue <- vector("integer", length(forest$trees[[numTree]]$treeMap))
    CutFeature <- vector("list", length(forest$trees[[numTree]]$treeMap))
    # nodeNum <- vector("integer", length(forest$trees[[numTree]]$treeMap))

    currentNode <- 0
    for (z in forest$trees[[numTree]]$treeMap) {
      currentNode <- currentNode + 1
      # nodeNum[currentNode] <- currentNode
      tmpPosition <- z
      if (tmpPosition > 0) {
        CutValue[currentNode] <- forest$trees[[numTree]]$CutPoint[forest$trees[[numTree]]$treeMap[currentNode]]
        firstFeaturePos <- forest$trees[[numTree]]$matAindex[tmpPosition] + 1
        secondFeaturePos <- forest$trees[[numTree]]$matAindex[tmpPosition + 1]
        CutFeature[[currentNode]] <- forest$trees[[numTree]]$matAstore[firstFeaturePos:secondFeaturePos]
      } else {
        tmpPosition <- -1 * tmpPosition
        CutValue[currentNode] <- NA
        CutFeature[[currentNode]] <- NA
      }
    }
    cutF[[numTree]] <- lapply(CutFeature, function(x) matrix(x, ncol = 2, byrow = TRUE)[, 2])
    cutV[[numTree]] <- CutValue
  }

  ## for the cut values determine if they in the appropriate bounds
  tVal <- list()
  for (ntree in 1:Ntrees) {
    tVal[[ntree]] <- list()
    for (node in 1:length(cutV[[ntree]])) {
      if (!is.null(cutV[[ntree]][[node]]) && !is.na(cutV[[ntree]][[node]])) {
        if (sum(cutF[[ntree]][[node]]) != 0) {
          cutRange <-
            c(min(0, sum(cutF[[ntree]][[node]])), max(0, sum(cutF[[ntree]][[node]])))
        } else {
          cutRange <-
            c(min(0, cutF[[ntree]][[node]]), max(cutF[[ntree]][[node]], 0))
        }

        tVal[[ntree]][node] <-
          cutV[[ntree]][[node]] > cutRange[1] & cutV[[ntree]][[node]] < cutRange[2]
      }
    }
  }

  ## Reduce the list and create a boolean that will be true if all of
  ## the cut values reside in the appropriate interval.
  testVal <- all(Reduce(c, Reduce(c, tVal)))
  expect_true(testVal)
})
