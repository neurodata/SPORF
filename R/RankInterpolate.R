RankInterpolate <-
  function(Xtrain, Xtest) {
    Xrank <- matrix(sapply(seq.int(ncol(Xtest)), FUN = function(cl) RankInterpolateVector(Xtrain[, cl], Xtest[, cl])), ncol = ncol(Xtest))
    colnames(Xrank) <- colnames(Xtest)
    return(Xrank)
  }
