#' Compute Feature Importance of a RerF model
#'
#' Computes feature importance of every unique feature used to make a split in the RerF model.
#'
#' @param forest a forest trained using the RerF function with argument store.impurity = TRUE
#' @param num.cores number of cores to use. If num.cores = 0, then 1 less than the number of cores reported by the OS are used. (num.cores = 0)
#'
#' @return feature.imp
#' 
#' @examples
#' library(rerf)
#' forest <- RerF(as.matrix(iris[, 1:4]), iris[[5L]], num.cores = 1L, store.impurity = TRUE)
#' feature.imp <- FeatureImportance(forest, num.cores = 1L)
#' 
#' @export
#' @importFrom parallel detectCores makeCluster clusterExport parSapply stopCluster
#' @importFrom utils object.size

FeatureImportance <- function(forest, num.cores = 0L) {
  num.trees <- length(forest$trees)
  num.splits <- sapply(forest$trees, function(tree) length(tree$CutPoint))
  
  unique.projections <- vector("list", sum(num.splits))
  
  idx.start <- 1L
  for (t in 1:num.trees) {
    idx.end <- idx.start + num.splits[t] - 1L
    unique.projections[idx.start:idx.end] <- lapply(1:num.splits[t], function(nd) forest$trees[[t]]$matAstore[(forest$trees[[t]]$matAindex[nd] + 1L):forest$trees[[t]]$matAindex[nd + 1L]])
    idx.start <- idx.end + 1L
  }
  unique.projections <- unique(unique.projections)
  
  CompImportanceCaller <- function(tree, ...) RunFeatureImportance(tree = tree, unique.projections = unique.projections)
  
  if (num.cores != 1L) {
    if (num.cores == 0L) {
      #Use all but 1 core if num.cores=0.
      num.cores=parallel::detectCores()-1L
    }
    num.cores <- min(num.cores, num.trees)
    gc()
    if ((utils::object.size(forest) > 2e9) |
        .Platform$OS.type == "windows") {
      
      cl <- parallel::makeCluster(spec = num.cores, type = "PSOCK")
      parallel::clusterExport(cl = cl, varlist = c("unique.projections", "RunFeatureImportance"), envir = environment())
      feature.imp <- parallel::parSapply(cl = cl, forest$trees, FUN = CompImportanceCaller)
      
    } else {
      cl <- parallel::makeCluster(spec = num.cores, type = "FORK")
      feature.imp <- parallel::parSapply(cl = cl, forest$trees, FUN = CompImportanceCaller)
    }
    
    parallel::stopCluster(cl)
    
  } else {
    #Use just one core.
    feature.imp <- sapply(forest$trees, FUN = CompImportanceCaller)
  }
  
  feature.imp <- apply(feature.imp, 1L, sum)
  sort.idx <- order(feature.imp, decreasing = TRUE)
  feature.imp <- feature.imp[sort.idx]
  unique.projections <- unique.projections[sort.idx]
  return(feature.imp <- list(imp = feature.imp, proj = unique.projections))
}
