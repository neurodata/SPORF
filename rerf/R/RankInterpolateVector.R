RankInterpolateVector <-
function(Xtrain, Xtest) {
  if (!is.matrix(Xtrain)) {
    Xtrain <- as.matrix(Xtrain)
  }
  if (!is.matrix(Xtest)) {
    Xtest <- as.matrix(Xtest)
  }
  ntrain <- nrow(Xtrain)
  p <- ncol(Xtrain)
  ntest <- nrow(Xtest)
  sort.idx <- apply(Xtrain, 2, order)
  sort.idx.test <- apply(Xtest, 2, order)
  Xtrain.rank <- RankMatrix(Xtrain)
  Xtest.rank <- matrix(0, nrow = ntest, ncol = p)
  for (j in seq.int(p)) {
    stidx <- 1L
    below.range <- T
    for (it in seq.int(ntest)) {
      if (below.range) {
        if (Xtrain[sort.idx[1L, j], j] > Xtest[sort.idx.test[it, j], j]) {
          Xtest.rank[sort.idx.test[it, j], j] <- 0
          next
        }
        below.range <- F
      }
      if (Xtrain[sort.idx[ntrain, j], j] < Xtest[sort.idx.test[it, j], j]) {
        Xtest.rank[sort.idx.test[it:ntest, j], j] <- ntrain + 1
        break
      }
      itr <- stidx
      while(Xtrain[sort.idx[itr, j], j] < Xtest[sort.idx.test[it, j], j]) {
        itr <- itr + 1L
      }
      stidx <- itr
      if (Xtrain[sort.idx[itr, j], j] == Xtest[sort.idx.test[it, j], j]) {
        Xtest.rank[sort.idx.test[it, j], j] <- Xtrain.rank[sort.idx[itr, j], j]
      } else {
        x.below <- Xtrain[sort.idx[itr - 1L, j], j]
        x.above <- Xtrain[sort.idx[itr, j], j]
        xr.below <- Xtrain.rank[sort.idx[itr - 1L, j], j]
        xr.above <- Xtrain.rank[sort.idx[itr, j], j]
        Xtest.rank[sort.idx.test[it, j], j] <- (Xtest[sort.idx.test[it, j], j] - x.below)/(x.above - x.below)*(xr.above - xr.below) + xr.below
      }
    }
  }
  return(Xtest.rank)
}
