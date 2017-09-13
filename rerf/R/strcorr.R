#' Compute strength and correlation of trees
#'
#' Returns estimates of the strength and correlation of the trees in a forest according to the definitions in Breiman 2001.
#'
#' @param Yhats ?????
#' @param Y ?????
#' @param nClasses ?????
#'
#' @return random.matrix ?????
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#' 
#' @export
#' @importFrom compiler setCompilerOptions cmpfun
#'

StrCorr <-
function(Yhats, Y, nClasses) {
  
      compiler::setCompilerOptions("optimize"=3)
    strcorr.comp <- compiler::cmpfun(run.strcorr)
  
  
  return(strcorr.comp(Yhats, Y, nClasses))
}
