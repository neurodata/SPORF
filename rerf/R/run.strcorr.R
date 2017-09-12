run.strcorr <-
function(Yhats, Y, nClasses) {
  n <- length(Y)
  nTrees <- ncol(Yhats)
  Ptheta <- matrix(0, nrow = n, ncol = nClasses)
  for (k in seq.int(nClasses)) {
    Ptheta[, k] <- apply(Yhats == k, 1L, sum)/nTrees
  }
  PthetaY <- Ptheta[1:n + (Y - 1L)*n]
  modeNotY <- apply(cbind(Yhats, Y), 1, function(x) sample.mode(x[1:nTrees][x[1:nTrees] != x[nTrees + 1L]]))
  PthetaNotY <- Ptheta[1:n + (modeNotY - 1L)*n]
  PthetaNotY[is.na(PthetaNotY)] <- 0
  
  strength <- mean(PthetaY - PthetaNotY)
  
  rmg <- matrix(0, nrow = n, ncol = nTrees)
  isallY <- is.na(modeNotY)
  rmg[isallY, ] <- Y[isallY]
  rmg[!isallY, ] <- apply(Yhats[!isallY, ], 2L, function(x) (x == Y[!isallY]) - (x == modeNotY[!isallY]))
  rho <- cor(rmg)
  sigma <- apply(rmg, 2L, sd)
  diag.idx <- seq(1, nTrees^2, nTrees + 1L)
  pairwise.sigma <- combn(nTrees, 2L, FUN = function(x) sigma[x[1]]*sigma[x[2L]])
  rho.bar <- mean(rho[lower.tri(rho)]*pairwise.sigma)/mean(pairwise.sigma)
  return(list(s = strength, rho = rho.bar))
}
