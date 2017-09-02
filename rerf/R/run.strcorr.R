run.strcorr <-
function(Yhats, Y, nClasses) {
  n <- length(Y)
  # Yhats <- sapply(predictmat, function(x) x[, 2])
  nTrees <- ncol(Yhats)
  Ptheta <- matrix(0, nrow = n, ncol = nClasses)
  for (k in seq.int(nClasses)) {
    Ptheta[, k] <- apply(Yhats == k, 1, sum)/nTrees
  }
  PthetaY <- Ptheta[1:n + (Y -1)*n]
  modeNotY <- apply(cbind(Yhats, Y), 1, function(x) sample.mode(x[1:nTrees][x[1:nTrees] != x[nTrees + 1]]))
  PthetaNotY <- Ptheta[1:n + (modeNotY - 1)*n]
  PthetaNotY[is.na(PthetaNotY)] <- 0
  
  strength <- mean(PthetaY - PthetaNotY)
  
  rmg <- matrix(0, nrow = n, ncol = nTrees)
  isallY <- is.na(modeNotY)
  rmg[isallY, ] <- Y[isallY]
  rmg[!isallY, ] <- apply(Yhats[!isallY, ], 2, function(x) (x == Y[!isallY]) - (x == modeNotY[!isallY]))
  rho <- cor(rmg)
  sigma <- apply(rmg, 2, sd)
  diag.idx <- seq(1, nTrees^2, nTrees + 1)
  pairwise.sigma <- combn(nTrees, 2, FUN = function(x) sigma[x[1]]*sigma[x[2]])
  rho.bar <- mean(rho[lower.tri(rho)]*pairwise.sigma)/mean(pairwise.sigma)
  return(list(s = strength, rho = rho.bar))
}
