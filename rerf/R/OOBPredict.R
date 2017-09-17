#' Compute out-of-bag prediction scores
#'
#' Computes out-of-bag class prediction scores for a forest trained with store.oob=TRUE.
#'
#' @param X an n sample by d feature matrix (preferable) or data frame which was used to train the provided forest.
#' @param forest a forest trained using the RerF function, with store.oob=TRUE.
#' @param num.cores the number of cores to use while training. If num.cores=0 then 1 less than the number of cores reported by the OS are used. (num.cores=0)
#'
#' @return scores
#'
#' @author James and Tyler, jbrowne6@jhu.edu and ttomita2@jhmi.edu
#' 
#' @examples
#' library(rerf)
#' X <- as.matrix(iris[,1:4])
#' Y <- as.numeric(iris[,5])
#' trainedForest <- RerF(X, Y, store.oob=TRUE, num.cores=1)
#' OOBPredict(X, trainedForest, num.cores=1)
#'
#' @export
#' @importFrom compiler setCompilerOptions cmpfun
#' @importFrom parallel detectCores makeCluster parLapply stopCluster
#'

OOBPredict <-
    function(X, forest, num.cores=0){
        if (!forest$params$store.oob) {
          stop("out-of-bag indices for each tree are not stored. RerF must be called with store.oob = TRUE in order to run OOBPredict.")
        }
      
        if (!is.matrix(X)) {
            X <- as.matrix(X)
        }
        if (forest$params$rank.transform) {
            X <- RankMatrix(X)
        }
        n <- nrow(X)

            compiler::setCompilerOptions("optimize"=3)
            CompOOB <- compiler::cmpfun(RunOOB)
            
        CompOOBCaller <- function(tree, ...) CompOOB(X = X, tree = tree)

        f_size <- length(forest$trees)
        if (num.cores != 1L) {
                if (num.cores == 0L) {
                    #Use all but 1 core if num.cores=0.
                    num.cores <- parallel::detectCores()-1L
                }
                #Start mclapply with num.cores Cores.
                num.cores <- min(num.cores, f_size)
                gc()
                if ((object.size(forest) > 2e9) | (object.size(X) > 2e9)) {
                    cl <- parallel::makeCluster(spec = num.cores, type = "PSOCK")
                    parallel::clusterExport(cl = cl, varlist = c("X", "CompOOB"), envir = environment())
                    Yhats <- parallel::parLapply(cl = cl, forest$trees, fun = CompOOBCaller)
                } else {
                    cl <- parallel::makeCluster(spec = num.cores, type = "FORK")
                    Yhats <- parallel::parLapply(cl = cl, forest$trees, fun = CompOOBCaller)
                }
                parallel::stopCluster(cl)
        } else {
            #Use just one core.
            Yhats <- lapply(forest$trees, FUN = CompOOBCaller)
        }
        num_classes <- ncol(forest$trees[[1L]]$ClassProb)
        # Have to make the last entry before this bottom will work.
        scores <- matrix(0,nrow=n, ncol=num_classes)
        oobCounts <- vector(mode = "integer", length = n)
        for(m in 1:f_size){
            oobCounts[forest$trees[[m]]$ind] <- oobCounts[forest$trees[[m]]$ind] + 1L
            for(k in 1:length(forest$trees[[m]]$ind)){
                scores[forest$trees[[m]]$ind[k], Yhats[[m]][k]] <- scores[forest$trees[[m]]$ind[k], Yhats[[m]][k]] + 1
            }
        }
        has.counts <- oobCounts != 0L
        scores[has.counts, ] <- scores[has.counts, ]/oobCounts[has.counts]
        return(scores)
    }