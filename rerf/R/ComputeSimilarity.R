#' Compute Similarities
#'
#' This is the ?????
#' 
#' @param X an n sample by d feature matrix (preferable) or data frame which was used to train the provided forest.
#' @param forest a forest trained using the rerf function, with COOB=TRUE.
#' @param num.cores the number of cores to use while training. If num.cores=0 then 1 less than the number of cores reported by the OS are used. (num.cores=0)
#' @param rank.transform ????? (rank.transform=FALSE)
#' @param Xtrain ????? (Xtrain=NULL)
#' 
#' @return ?????
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#' 
#' @examples
#' library(rerf)
#' X <- as.matrix(iris[,1:4])
#' trainedForest <- rerf(X, as.numeric(iris[,5]), num.cores=1)
#' sim.matrix <- ComputeSimilarity(X, trainedForest, num.cores=1)
#'
#' @export
#' @importFrom compiler setCompilerOptions cmpfun
#' @importFrom parallel detectCores makeCluster clusterExport parLapply stopCluster
#'


ComputeSimilarity <-
function(X, forest, num.cores=0, rank.transform = F, Xtrain = NULL){
  if (!is.matrix(X)) {
    X <- as.matrix(X)
  }
  if (rank.transform) {
    if (is.null(Xtrain)) {
      ############ error ############
      stop("The model was trained on rank-transformed data. Xtrain must be provided in order to embed Xtest into the rank space")
    } else {
      X <- RankInterpolate(Xtrain, X)
    }
  }
  
  n <- nrow(X)
  
    compiler::setCompilerOptions("optimize"=3)
    comp_predict.leaf <- compiler::cmpfun(runpredict.leaf)
  
  comp_predict_caller <- function(tree, ...) comp_predict.leaf(X=X, tree=tree)
  
  f_size <- length(forest)
  if(num.cores!=1){
      if(num.cores==0){
        #Use all but 1 core if num.cores=0.
        num.cores=parallel::detectCores()-1L
      }
      #Start mclapply with num.cores Cores.
      num.cores <- min(num.cores, f_size)
      gc()
      if ((object.size(forest) > 2e9) | (object.size(X) > 2e9)) {
        cl <- parallel::makeCluster(spec = num.cores, type = "PSOCK")
        parallel::clusterExport(cl = cl, varlist = c("X", "comp_predict.leaf"), envir = environment())
        leafIdx <- parallel::parLapply(cl = cl, forest, fun = comp_predict_caller)
      } else {
        cl <- parallel::makeCluster(spec = num.cores, type = "FORK")
        leafIdx <- parallel::parLapply(cl = cl, forest, fun = comp_predict_caller)
      }
      parallel::stopCluster(cl)
      
  }else{
    #Use just one core.
    leafIdx <- lapply(forest, FUN = comp_predict_caller)
  }
  
  leafIdx <- matrix(unlist(leafIdx), nrow = n, ncol = f_size)
  
  similarity <- matrix(0, nrow = n, ncol = n)
  
  for (m in 1:f_size) {
    sortIdx <- order(leafIdx[, m])
    nLeaf <- nrow(forest[[m]]$ClassProb)
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
