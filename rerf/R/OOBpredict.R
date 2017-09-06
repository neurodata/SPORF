OOBpredict <-
function(X, Forest, NumCores=0, rank.transform = F, comp.mode = "batch"){
  if (!is.matrix(X)) {
    X <- as.matrix(X)
  }
  if (rank.transform){
    X <- rank.matrix(X)
  }
  n <- nrow(X)
  
  if(!requireNamespace(compiler)){
    cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to find the OOB error rate.\n")
    comp_errOOB <<- runerrOOB
  }
  
  if(!exists("comp_errOOB")){
      compiler::setCompilerOptions("optimize"=3)
    comp_errOOB <<- compiler::cmpfun(runerrOOB)
  } 
  comp_errOOB_caller <- function(tree, ...) comp_errOOB(X = X, tree = tree, comp.mode = comp.mode)
  
  f_size <- length(Forest)
  if(NumCores!=1){
    if(requireNamespace(doParallel)){
      if(NumCores==0){
        #Use all but 1 core if NumCores=0.
        NumCores=parallel::detectCores()-1L
      }
      #Start mclapply with NumCores Cores.
      NumCores <- min(NumCores, f_size)
      gc()
      if ((object.size(Forest) > 2e9) | (object.size(X) > 2e9)) {
        cl <<- makeCluster(spec = NumCores, type = "PSOCK")
        clusterExport(cl = cl, varlist = c("X", "comp.mode", "comp_errOOB"), envir = environment())
        Yhats <- parLapply(cl = cl, Forest, fun = comp_errOOB_caller)
      } else {
        cl <<- makeCluster(spec = NumCores, type = "FORK")
        Yhats <- parLapply(cl = cl, Forest, fun = comp_errOOB_caller)
      }
      stopCluster(cl)
    }else{
      #Parallel package not available.
      cat("Package 'parallel' not available.\nExecution will continue without parallelization.\nThis will increase the time required to create the forest\n")
      Yhats <- lapply(Forest, FUN = comp_errOOB_caller)
    }
  }else{
    #Use just one core.
    Yhats <- lapply(Forest, FUN = comp_errOOB_caller)
  }
  
  num_classes <- ncol(Forest[[1]]$ClassProb)
  # Have to make the last entry before this bottom will work.
  scores <- matrix(0,nrow=n, ncol=num_classes)
  oobCounts <- vector(mode = "integer", length = n)
  for(m in 1:f_size){
    oobCounts[Forest[[m]]$ind] <- oobCounts[Forest[[m]]$ind] + 1L
    # scores[Forest[[m]]$ind] <- scores[Forest[[m]]$ind] + Yhats[[m]]
    for(k in 1:length(Forest[[m]]$ind)){
      scores[Forest[[m]]$ind[k], Yhats[[m]][k]] <- scores[Forest[[m]]$ind[k], Yhats[[m]][k]] + 1
    }
  }
  has.counts <- oobCounts != 0L
  scores[has.counts, ] <- scores[has.counts, ]/oobCounts[has.counts]
  return(scores)
}
