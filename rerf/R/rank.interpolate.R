rank.interpolate <-
function(Xtrain, Xtest) {
  if (!require(compiler)) {
    compRank <<- runRankInterpolate
  }
  
  if (!exists("compRank")) {
    setCompilerOptions("optimize" = 3)
    compRank <<- cmpfun(runRankInterpolate)
  }
  
  return(sapply(seq.int(ncol(Xtest)), FUN = function(cl) compRank(Xtrain[, cl], Xtest[, cl])))
}
