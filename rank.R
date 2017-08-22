rank.matrix <- function(X, na.last = T, ties.method = "average") {
  if (is.matrix(X)) {
    X.rank <- apply(X, 2, FUN = function(x) rank(x, na.last = na.last, ties.method = ties.method))
  } else {
    X.rank <- rank(X, na.last = na.last, ties.method = ties.method)
  }
  return(X.rank)
}

rank.interpolate <- function(Xtrain, Xtest) {
  if (!is.matrix(Xtrain)) {
    Xtrain <- as.matrix(Xtrain)
  }
  if (!is.matrix(Xtest)) {
    Xtest <- as.matrix(Xtest)
  }
  ntrain <- nrow(Xtrain)
  p <- ncol(Xtrain)
  train.idx <- 1:ntrain
  ntest <- nrow(Xtest)
  n <- ntrain + ntest
  test.idx <- (ntrain + 1L):n
  X <- rbind(Xtrain, Xtest)
  sort.idx <- apply(X, 2, function(x) sort(x, index.return = T)$ix)
  Xtrain.rank <- rank.matrix(Xtrain)
  Xtest.rank <- matrix(0, nrow = ntest, ncol = p)
  for (j in 1:p) {
    i <- 1L
    sort.train.idx <- sort.idx[, j] <= ntrain
    sort.test.idx <- which(!sort.train.idx)
    sort.train.idx <- which(sort.train.idx)
    min.idx <- sort.idx[sort.train.idx[1], j]
    min.train <- Xtrain[min.idx, j]
    min.train.rank <- Xtrain.rank[min.idx, j]
    max.idx <- sort.idx[sort.train.idx[ntrain], j]
    max.train <- Xtrain[max.idx, j]
    max.train.rank <- Xtrain.rank[max.idx, j]
    for (ix in sort.test.idx) {
      i <- sort.idx[ix, j]
    # for (i in sort.idx[sort.test.idx, j]) {
      i.test <- i - ntrain
      if (Xtest[i.test, j] < min.train) {
        Xtest.rank[i.test, j] <- 0
      } else if (Xtest[i.test, j] == min.train) {
        Xtest.rank[i.test, j] <- min.train.rank
      } else if (Xtest[i.test, j] > max.train) {
        Xtest.rank[i.test, j] <- ntrain + 1
      } else if (Xtest[i.test, j] == max.train) { 
        Xtest.rank[i.test, j] <- max.train.rank
      } else {
        ix.below <- 0L
        is.test <- T
        while (is.test) {
          ix.below <- ix.below + 1L
          is.test <- sort.idx[ix - ix.below, j] > ntrain
        }
        ix.above <- 0L
        is.test <- T
        while (is.test) {
          ix.above <- ix.above + 1L
          is.test <- sort.idx[ix + ix.above, j] > ntrain
        }
        i.below <- sort.idx[ix - ix.below, j]
        i.above <- sort.idx[ix + ix.above, j]
        if (Xtest[i.test, j] == X[i.above, j]) {
          Xtest.rank[i.test, j] <- Xtrain.rank[i.above, j]
        } else if (Xtest[i.test, j] == X[i.below, j]) {
          Xtest.rank[i.test, j] <- Xtrain.rank[i.below, j]
        } else {
          Xtest.rank[i.test, j] <- ((X[i, j] - X[i.below, j])/(X[i.above, j] - X[i.below, j]))*(Xtrain.rank[i.above, j] - Xtrain.rank[i.below, j]) + Xtrain.rank[i.below, j]
        }
      }
    }
  }
  return(Xtest.rank)
}