#' Calculate Error Rate of Trained Forest
#'
#' This function provides an error rate for a given sample and corresponding class values. 
#'
#' @param X an n sample by d feature matrix (preferable) or data frame
#' @param Y an n length vector of class labels.  Class labels must be numeric and be within the range 1 to the number of classes.
#' @param forest a trained forest returned by the rerf function.
#' @param num.cores the number of cores to use while training. If num.cores=0 then 1 less than the number of cores reported by the OS are used. (num.cores=0)
#'
#' @return errorRate
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#' 
#' @examples
#' library(rerf)
#' X <- as.matrix(iris[,1:4])
#' Y <- as.numeric(iris[,5])
#' trainedForest <- rerf(X, Y, 5, num.cores=1)
#' # Using a test X and Y
#' ErrorRate(X, Y, trainedForest, num.cores=1)
#'
#' @export
#' @importFrom compiler setCompilerOptions cmpfun
#' @importFrom parallel detectCores mclapply
#'

ErrorRate <-
function(X,Y,forest, num.cores=0L){
  
      compiler::setCompilerOptions("optimize"=3)
    comp_err <- compiler::cmpfun(RunErr)
  
  X<- as.matrix(X)
  if(num.cores!=1){
      if(num.cores==0){
        #Use all but 1 core if num.cores=0.
        num.cores=parallel::detectCores()-1L
      }
      #Start mclapply with num.cores Cores.
      if (nrow(X)%%num.cores==0){
        chunk_size <- nrow(X)/num.cores
        comp_err_caller <- function(z, ...) comp_err(X=X,Y=Y,Forest=forest,index=z, chunk_size=chunk_size)
        gc()
        total_misclassified <- sum(as.numeric(parallel::mclapply(1L:num.cores,comp_err_caller, mc.cores=num.cores)))
      }else{
        if(nrow(X) > num.cores){
          chunk_size <- floor(nrow(X)/num.cores)
          comp_err_caller <- function(z, ...) comp_err(X=X,Y=Y,Forest=forest,index=z, chunk_size=chunk_size)
          gc()
          total_misclassified <- sum(as.numeric(parallel::mclapply(1L:num.cores,comp_err_caller, mc.cores=num.cores)))
        }
        comp_err_caller <- function(z, ...) comp_err(X=X[(num.cores*chunk_size+1L):nrow(X),,drop=FALSE],Y=Y[(num.cores*chunk_size+1L):nrow(X)],Forest=forest,index=z, chunk_size=1L)
        gc()
        total_misclassified <- total_misclassified + sum(as.numeric(parallel::mclapply(1L:(nrow(X)%%num.cores), comp_err_caller, mc.cores=(nrow(X)%%num.cores))))
      }
   
  }else{
    #Use just one core.
    total_misclassified <-comp_err(X, Y, forest)
  }
  return(total_misclassified/nrow(X))
}
