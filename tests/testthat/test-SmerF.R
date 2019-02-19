context("SmerF")
library(rerf)

set.seed(123456)

################################################################
### create simulated data with a simple similarity structure ###
################################################################

# points lie on one of two axes, or at the origin
# points that strictly lie on the same axis have a similarity of 1 to each others
# points that lie on different axes have a similarity of 0
# points on either axis have a similarity of 0.5 to points at the origin
# points have a similarity of 1 with themselves (diagonals)

# input matrix is 2 features, 5 samples
X <- rbind(c(0, 0), c(1, 0), c(0, 1), c(1.1, 0), c(0, 0.5), c(0, 0))
n <- nrow(X)
p <- ncol(X)
Y <- diag(n)
pairs <- combn(n, 2)
for (i in 1:ncol(pairs)) {
  idx1 <- pairs[1, i]
  idx2 <- pairs[2, i]
  if (identical(X[idx1, ], c(0, 0)) && identical(X[idx2, ], c(0, 0))) {
    Y[idx1, idx2] <- Y[idx2, idx1] <- 1
  } else if (identical(X[idx1, ], c(0, 0)) || identical(X[idx2, ], c(0, 0))) {
    Y[idx1, idx2] <- Y[idx2, idx1] <- 0.5
  } else if (X[idx1, ] %*% X[idx2, ] == 0) {
    Y[idx1, idx2] <- Y[idx2, idx1] <- 0
  } else {
    Y[idx1, idx2] <- Y[idx2, idx1] <- 1
  }
}


test_that("findSplitSim finds the correct split point and variable", {
  I <- mean(Y[lower.tri(Y, diag = TRUE)]) # "information" of the parent node
  splits <- list(MaxDeltaI = 0, BestVar = rep(0L, n), BestSplit = rep(0, n), NumBest = 0L)

  # iterate over the two features and find the best split
  for (feat in 1:p) {
    # sort the values for the current feature
    sort_idx <- order(X[, feat])
    x <- X[sort_idx, feat]
    y <- Y[sort_idx, sort_idx]
    splits[] <- rerf:::findSplitSim(
      x = x,
      y = y,
      ndSize = n,
      I = I,
      maxdI = splits$MaxDeltaI,
      bv = splits$BestVar,
      bs = splits$BestSplit,
      nb = splits$NumBest,
      nzidx = feat
    )
  }
  splits$BestVar <- splits$BestVar[1:splits$NumBest]
  splits$BestSplit <- splits$BestSplit[1:splits$NumBest]

  expected_deltaI <- (1 / 3) * mean(Y[c(3, 5), c(3, 5)][lower.tri(Y[c(3, 5), c(3, 5)], diag = T)]) +
    (2 / 3) * mean(Y[-c(3, 5), -c(3, 5)][lower.tri(Y[-c(3, 5), -c(3, 5)], diag = T)]) -
    I
  # By design, two best splits expected: one on the first dimension at 0.5 and one on the second dimension at 0.25
  expected_output <- list(
    MaxDeltaI = expected_deltaI,
    BestVar = c(1, 2),
    BestSplit = c(0.5, 0.25),
    NumBest = 2L
  )

  expect_equal(splits, expected_output)
})



test_that("forest object has the correct member names and values", {
  # we will just train one axis-aligned tree with no subsampling of observations or features

  forest <- RerF(X, Y,
    FUN = RandMatRF,
    paramList = list(p = p, d = p),
    trees = 1L,
    bagging = 0,
    task = "similarity",
    seed = 1L
  )

  expected_output <- list(
    trees = list(list(
      treeMap = c(1L, 2L, -3L, -1L, -2L),
      CutPoint = c(0.5, 0.25),
      leafSimilarity = matrix(c(1, 0.5, 0.5, 0, 1, 0, 0, 0, 1), nrow = 3L),
      matAstore = c(1L, 1L, 2L, 1L),
      matAindex = c(0L, 2L, 4L),
      ind = NULL,
      rotmat = NULL,
      rotdims = NULL,
      delta.impurity = NULL
    )),
    labels = NULL,
    params = list(
      min.parent = 1L,
      max.depth = 0L,
      bagging = 0,
      replacement = TRUE,
      stratify = TRUE,
      fun = RandMatRF,
      paramList = list(
        p = 2L,
        d = 2L,
        sparsity = 0.5,
        prob = 0.5
      ),
      rank.transform = FALSE,
      store.oob = FALSE,
      store.impurity = FALSE,
      rotate = FALSE,
      seed = 1L,
      task = "similarity",
      eps = 0.05
    )
  )
  expect_equal(forest, expected_output)
})


test_that("Predict produces the right size and shape output for the various cases", {

  # generate test set
  Xtest <- rbind(c(0.1, 0), c(0, 0.1), c(2, 0), c(0, 2))
  ntest <- nrow(Xtest)

  # train forest of 100 trees
  forest <- RerF(X, Y,
    FUN = RandMatRF,
    paramList = list(p = p, d = p),
    trees = 100L,
    store.oob = TRUE,
    task = "similarity",
    seed = 1L
  )

  # predict the similarities between all pairs of OOB samples
  Yhat <- Predict(X, forest, OOB = TRUE, num.cores = 1L)

  # shape of predicted similarity matrix should be ntrain-by-ntrain
  expect_equal(dim(Yhat), c(n, n))

  # predict the similarities between test samples and OOB samples
  Yhat <- Predict(Xtest, forest, OOB = TRUE, Xtrain = X)

  # shape of predicted similarity matrix should be ntest-by-ntrain
  expect_equal(dim(Yhat), c(ntest, n))

  # predict the similarities between test samples and all training samples
  Yhat <- Predict(Xtest, forest, OOB = FALSE, Xtrain = X)

  # shape of predicted similarity matrix should be ntest-by-ntrain
  expect_equal(dim(Yhat), c(ntest, n))

  # predict the similarities between all pairs of test samples
  Yhat <- Predict(Xtest, forest, OOB = FALSE)

  # shape of predicted similarity matrix should be ntest-by-ntest
  expect_equal(dim(Yhat), c(ntest, ntest))
})
