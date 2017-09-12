#' Create a strcorr 
#'
#' Create a random matrix using given params.
#'
#' @param Yhats ?????
#' @param Y ?????
#' @param nClasses ?????
#'
#' @return random.matrix ?????
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#' 
#' @importFrom compiler setCompilerOptions cmpfun
#'

strcorr <-
function(Yhats, Y, nClasses) {
  
      compiler::setCompilerOptions("optimize"=3)
    strcorr.comp <- compiler::cmpfun(run.strcorr)
  
  
  return(strcorr.comp(Yhats, Y, nClasses))
}
