#' Compute tree strength and correlation
#'
#' Computes estimates of tree strength and correlation according to the definitions in Breiman's 2001 Random Forests paper.
#'
#' @param Yhats predicted class labels for each tree in a forest.
#' @param Y true class labels.
#'
#' @return scor
#'
#' @author James Browne and Tyler Tomita, jbrowne6@jhu.edu and ttomita2@jhmi.edu
#' 
#' @export
#'
#' @examples
#' library(rerf)
#' trainIdx <- c(1:40, 51:90, 101:140)
#' X <- as.matrix(iris[,1:4])
#' Y <- iris[[5]]
#' forest <- RerF(X[trainIdx, ], Y[trainIdx], num.cores = 1L)
#' predictions <- Predict(X[-trainIdx, ], forest, num.cores = 1L, aggregate.output = FALSE)
#' scor <- StrCorr(predictions, Y[-trainIdx])

StrCorr <-
function(Yhats, Y) {
  if (is.factor(Y)) {
    cl.labels <- unique(c(Yhats, as.character(Y)))
  } else {
    cl.labels <- unique(c(Yhats, Y))
  }
  nClasses <- length(cl.labels)
  Yhats <- matrix(as.integer(factor(Yhats, levels = cl.labels)), nrow = nrow(Yhats), ncol = ncol(Yhats))
  Y <- as.integer(factor(Y, levels = cl.labels))
  n <- length(Y)
  nTrees <- ncol(Yhats)
  Ptheta <- matrix(0, nrow = n, ncol = nClasses)
  for (k in seq.int(nClasses)) {
    Ptheta[, k] <- apply(Yhats == k, 1L, sum)/nTrees
  }
  PthetaY <- Ptheta[1:n + (Y - 1L)*n]
  modeNotY <- apply(cbind(Yhats, Y), 1, function(x) SampleMode(x[1L:nTrees][x[1L:nTrees] != x[nTrees + 1L]]))
  PthetaNotY <- Ptheta[1L:n + (modeNotY - 1L)*n]
  PthetaNotY[is.na(PthetaNotY)] <- 0
  
  strength <- mean(PthetaY - PthetaNotY)
  
  rmg <- matrix(0, nrow = n, ncol = nTrees)
  isallY <- is.na(modeNotY)
  rmg[isallY, ] <- Y[isallY]
  rmg[!isallY, ] <- apply(Yhats[!isallY, , drop = F], 2L, function(x) (x == Y[!isallY]) - (x == modeNotY[!isallY]))
  rho <- cor(rmg)
  sigma <- apply(rmg, 2L, sd)
  diag.idx <- seq(1, nTrees^2, nTrees + 1L)
  pairwise.sigma <- combn(nTrees, 2L, FUN = function(x) sigma[x[1L]]*sigma[x[2L]])
  rho.bar <- mean(rho[lower.tri(rho)]*pairwise.sigma)/mean(pairwise.sigma)
  scor <- list(s = strength, rho = rho.bar)
  return(scor)
}
