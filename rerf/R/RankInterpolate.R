#' Interpolate Rank 
#'
#' Determines the OOB ????? error rate for a forest trained with COOB=TRUE.
#'
#' @param Xtrain ?????
#' @param Xtest ????? 
#'
#' @return Rate?????
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#' 
#' @importFrom compiler setCompilerOptions cmpfun
#'


RankInterpolate <-
function(Xtrain, Xtest) {
  
    compiler::setCompilerOptions("optimize" = 3)
    compRank <- compiler::cmpfun(runRankInterpolate)
  
  return(sapply(seq.int(ncol(Xtest)), FUN = function(cl) compRank(Xtrain[, cl], Xtest[, cl])))
}
