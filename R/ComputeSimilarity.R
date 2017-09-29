#' Compute Similarities
#'
#' Computes pairwise similarities between observations. The similarity between two points is defined as the fraction of trees such that two points fall into the same leaf node.
#' 
#' @param X an n sample by d feature matrix (preferable) or data frame which was used to train the provided forest.
#' @param forest a forest trained using the rerf function, with COOB=TRUE.
#' @param num.cores the number of cores to use while training. If num.cores=0 then 1 less than the number of cores reported by the OS are used. (num.cores=0)
#' @param Xtrain  an n by d numeric matrix (preferable) or data frame. This should be the same data matrix/frame used to train the forest, and is only required if RerF was called with rank.transform = TRUE. (Xtrain=NULL)
#' 
#' @return similarity a normalized n by n matrix of pairwise similarities
#'
#' @author James Browne (jbrowne6@jhu.edu) and Tyler Tomita (ttomita2@jhmi.edu) 
#' 
#' @examples
#' library(rerf)
#' X <- as.matrix(iris[,1:4])
#' Y <- iris[[5L]]
#' forest <- RerF(X, Y, num.cores = 1L)
#' sim.matrix <- ComputeSimilarity(X, forest, num.cores = 1L)
#'
#' @export
#' @importFrom parallel detectCores makeCluster clusterExport parLapply stopCluster
#' @importFrom utils object.size combn
#'


ComputeSimilarity <-
    function(X, 
             forest, 
             num.cores = 0L, 
             Xtrain = NULL){

        if (!is.matrix(X)) {
            X <- as.matrix(X)
        }
        if (forest$params$rank.transform) {
            if (is.null(Xtrain)) {
                ############ error ############
                stop("The model was trained on rank-transformed data. Xtrain must be provided in order to embed Xtest into the rank space")
            } else {
                X <- RankInterpolate(Xtrain, X)
            }
        }

        n <- nrow(X)


        CompPredictCaller <- function(tree, ...) RunPredictLeaf(X=X, tree=tree)

        f_size <- length(forest$trees)
        # run single core or multicore
        if(num.cores != 1L){
            # Use all but 1 core if num.cores=0.
            if(num.cores == 0L){
                num.cores=parallel::detectCores()-1L
            }
            num.cores <- min(num.cores, f_size)
            gc()
            #Start cluster with num.cores cores
            if ((utils::object.size(forest) > 2e9) || (utils::object.size(X) > 2e9)) {
              cl <- parallel::makeCluster(spec = num.cores, type = "PSOCK")
              parallel::clusterExport(cl = cl, varlist = c("X", "RunPredictLeaf"), envir = environment())
              leafIdx <- parallel::parLapply(cl = cl, forest$trees, fun = CompPredictCaller)
            } else {
              cl <- parallel::makeCluster(spec = num.cores, type = "FORK")
              Yhats <- parallel::parLapply(cl = cl, forest$trees, fun = CompPredictCaller)
            }
            parallel::stopCluster(cl)

        }else{
            #Use just one core.
            leafIdx <- lapply(forest$trees, FUN = CompPredictCaller)
        }

        leafIdx <- matrix(unlist(leafIdx), nrow = n, ncol = f_size)

        similarity <- matrix(0, nrow = n, ncol = n)

        for (m in 1L:f_size) {
            sortIdx <- order(leafIdx[, m])
            nLeaf <- nrow(forest$trees[[m]]$ClassProb)
            leafCounts <- tabulate(leafIdx[, m], nLeaf)
            leafCounts.cum <- cumsum(leafCounts)
            if (leafCounts[1L] > 1L) {
                prs <- utils::combn(sort(sortIdx[seq.int(leafCounts[1L])]), 2L)
                idx <- (prs[1L, ] - 1L)*n + prs[2L, ]
                similarity[idx] <- similarity[idx] + 1L
            }
            for (k in seq.int(nLeaf - 1L) + 1L) {
                if (leafCounts[k] > 1L) {
                    prs <- utils::combn(sort(sortIdx[(leafCounts.cum[k - 1L] + 1L):leafCounts.cum[k]]), 2L)
                    idx <- (prs[1L, ] - 1L)*n + prs[2L, ]
                    similarity[idx] <- similarity[idx] + 1L
                }
            }
        }
        similarity <- similarity + t(similarity)
        diag(similarity) <- f_size
        similarity <- similarity/f_size
        return(similarity)
    }
