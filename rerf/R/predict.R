predict <-
function(X, Forest, NumCores=0, rank.transform = F, Xtrain = NULL, comp.mode = "batch", out.mode = "aggregate"){
  if (!is.matrix(X)) {
    X <- as.matrix(X)
  }
  if (rank.transform) {
    if (is.null(Xtrain)) {
      ############ error ############
      stop("The model was trained on rank-transformed data. Xtrain must be provided in order to embed Xtest into the rank space")
    } else {
      X <- rank.interpolate(Xtrain, X)
    }
  }
  
  if(!requireNamespace(compiler)){
    cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to predict.\n")
    comp_predict <<- runpredict
  }
  
  if(!exists("comp_predict")){
    setCompilerOptions("optimize"=3)
    comp_predict <<- cmpfun(runpredict)
  } 
  
  comp_predict_caller <- function(tree, ...) comp_predict(X=X, tree=tree, comp.mode = comp.mode)
  
  f_size <- length(Forest)
  if(NumCores!=1){
    if(requireNamespace(parallel)){
      if(NumCores==0){
        #Use all but 1 core if NumCores=0.
        NumCores=detectCores()-1L
      }
      #Start mclapply with NumCores Cores.
      NumCores <- min(NumCores, f_size)
      gc()
      if ((object.size(Forest) > 2e9) | (object.size(X) > 2e9)) {
        cl <- makeCluster(spec = NumCores, type = "PSOCK")
        clusterExport(cl = cl, varlist = c("X", "comp.mode", "comp_predict"), envir = environment())
        Yhats <- parLapply(cl = cl, Forest, fun = comp_predict_caller)
      } else {
        cl <- makeCluster(spec = NumCores, type = "FORK")
        Yhats <- parLapply(cl = cl, Forest, fun = comp_predict_caller)
      }
      stopCluster(cl)
    }else{
      #Parallel package not available.
      cat("Package 'parallel' not available.\nExecution will continue without parallelization.\nThis will increase the time required to predict.\n")
      Yhats <- lapply(Forest, FUN = comp_predict_caller)
    }
  }else{
    #Use just one core.
    Yhats <- lapply(Forest, FUN = comp_predict_caller)
  }
  
  if (out.mode == "individual") {
    return(matrix(unlist(Yhats), nrow(X), f_size))
  } else {
    num_classes <- ncol(Forest[[1]]$ClassProb)
    scores <- matrix(0,nrow=nrow(X), ncol=num_classes)
    for(m in 1:f_size){
      # scores <- scores + Yhats[[m]]
      for(k in 1:nrow(X)){
        scores[k, Yhats[[m]][k]] <- scores[k, Yhats[[m]][k]] + 1
      }
    }
    scores <- scores/f_size
    return(scores)
  }
}
