strcorr <-
function(Yhats, Y, nClasses) {
  if(!requireNamespace(compiler)){
    cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to compute strength and correlation.\n")
    strcorr.comp <<- run.strcorr
  }
  
  if(!exists("strcorr.comp")){
      compiler::setCompilerOptions("optimize"=3)
    strcorr.comp <<- compiler::cmpfun(run.strcorr)
  }
  
  return(strcorr.comp(Yhats, Y, nClasses))
}
