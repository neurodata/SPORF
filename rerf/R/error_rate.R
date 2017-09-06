error_rate <-
function(X,Y,Forest, NumCores=0L){
  if(!is.null(Forest$forest)){
    Forest<-Forest$forest
  }
  if(!requireNamespace(compiler)){
    cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to find the error rate.\n")
    comp_err <<- RunErr
  }
  
  if(!exists("comp_err")){
    setCompilerOptions("optimize"=3)
    comp_err <<- cmpfun(RunErr)
  } 
  
  X<- as.matrix(X)
  if(NumCores!=1){
    if(requireNamespace(parallel)){
      if(NumCores==0){
        #Use all but 1 core if NumCores=0.
        NumCores=detectCores()-1L
      }
      #Start mclapply with NumCores Cores.
      if (nrow(X)%%NumCores==0){
        chunk_size <- nrow(X)/NumCores
        comp_err_caller <- function(z, ...) comp_err(X=X,Y=Y,Forest=Forest,index=z, chunk_size=chunk_size)
        gc()
        total_misclassified <- sum(as.numeric(mclapply(1L:NumCores,comp_err_caller, mc.cores=NumCores)))
      }else{
        if(nrow(X) > NumCores){
          chunk_size <- floor(nrow(X)/NumCores)
          comp_err_caller <- function(z, ...) comp_err(X=X,Y=Y,Forest=Forest,index=z, chunk_size=chunk_size)
          gc()
          total_misclassified <- sum(as.numeric(mclapply(1L:NumCores,comp_err_caller, mc.cores=NumCores)))
        }
        comp_err_caller <- function(z, ...) comp_err(X=X[(NumCores*chunk_size+1L):nrow(X),,drop=FALSE],Y=Y[(NumCores*chunk_size+1L):nrow(X)],Forest=Forest,index=z, chunk_size=1L)
        gc()
        total_misclassified <- total_misclassified + sum(as.numeric(mclapply(1L:(nrow(X)%%NumCores), comp_err_caller, mc.cores=(nrow(X)%%NumCores))))
      }
    }else{
      #Parallel package not available.
      cat("Package 'parallel' not available.\nExecution will continue without parallelization.\nThis will increase the time required to create the forest\n")
      total_misclassified <-comp_err(X, Y, Forest)
    }
  }else{
    #Use just one core.
    total_misclassified <-comp_err(X, Y, Forest)
  }
  return(total_misclassified/nrow(X))
}
