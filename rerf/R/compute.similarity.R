compute.similarity <-
function(X, Forest, NumCores=0, rank.transform = F, Xtrain = NULL){
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
  
  n <- nrow(X)
  
  if(!requireNamespace(compiler)){
    cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to predict.\n")
    comp_predict.leaf <<- runpredict.leaf
  }
  
  if(!exists("comp_predict.leaf")){
    setCompilerOptions("optimize"=3)
    comp_predict.leaf <<- cmpfun(runpredict.leaf)
  } 
  
  comp_predict_caller <- function(tree, ...) comp_predict.leaf(X=X, tree=tree)
  
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
        clusterExport(cl = cl, varlist = c("X", "comp_predict.leaf"), envir = environment())
        leafIdx <- parLapply(cl = cl, Forest, fun = comp_predict_caller)
      } else {
        cl <- makeCluster(spec = NumCores, type = "FORK")
        leafIdx <- parLapply(cl = cl, Forest, fun = comp_predict_caller)
      }
      stopCluster(cl)
    }else{
      #Parallel package not available.
      cat("Package 'parallel' not available.\nExecution will continue without parallelization.\nThis will increase the time required to predict.\n")
      leafIdx <- lapply(Forest, FUN = comp_predict_caller)
    }
  }else{
    #Use just one core.
    leafIdx <- lapply(Forest, FUN = comp_predict_caller)
  }
  
  leafIdx <- matrix(unlist(leafIdx), nrow = n, ncol = f_size)
  
  similarity <- matrix(0, nrow = n, ncol = n)
  
  for (m in 1:f_size) {
    sortIdx <- order(leafIdx[, m])
    nLeaf <- nrow(Forest[[m]]$ClassProb)
    leafCounts <- tabulate(leafIdx[, m], nLeaf)
    leafCounts.cum <- cumsum(leafCounts)
    if (leafCounts[1L] > 1L) {
      prs <- combn(sort(sortIdx[seq.int(leafCounts[1L])]), 2L)
      idx <- (prs[1L, ] - 1L)*n + prs[2L, ]
      similarity[idx] <- similarity[idx] + 1
    }
    for (k in seq.int(nLeaf - 1L) + 1L) {
      if (leafCounts[k] > 1L) {
        prs <- combn(sort(sortIdx[(leafCounts.cum[k - 1L] + 1L):leafCounts.cum[k]]), 2L)
        idx <- (prs[1L, ] - 1L)*n + prs[2L, ]
        similarity[idx] <- similarity[idx] + 1
      }
    }
  }
  
  similarity <- similarity + t(similarity)
  diag(similarity) <- f_size
  
  return(similarity/f_size)
}
