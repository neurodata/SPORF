strcorr <-
function(Yhats, Y, nClasses) {
  if(!require(compiler)){
    cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to compute strength and correlation.\n")
    strcorr.comp <<- run.strcorr
  }
  
  if(!exists("strcorr.comp")){
    setCompilerOptions("optimize"=3)
    strcorr.comp <<- cmpfun(run.strcorr)
  }
  
  return(strcorr.comp(Yhats, Y, nClasses))
}
