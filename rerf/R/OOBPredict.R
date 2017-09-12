#' OOB Error Rate 
#'
#' Determines the OOB error rate for a forest trained with COOB=TRUE.
#'
#' @param X an n sample by d feature matrix (preferable) or data frame which was used to train the provided forest.
#' @param forest a forest trained using the rerf function, with COOB=TRUE.
#' @param num.cores the number of cores to use while training. If num.cores=0 then 1 less than the number of cores reported by the OS are used. (num.cores=0)
#' @param rank.transform ????? (rank.transform=FALSE)
#' @param comp.mode ????? (comp.mode="batch")
#'
#' @return OOBErrorRate
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#' 
#' @examples
#' library(rerf)
#' X <- as.matrix(iris[,1:4])
#' Y <- as.numeric(iris[,5])
#' trainedForest <- rerf(X, Y, COOB=TRUE, num.cores=1)
#' OOBPredict(X, trainedForest, num.cores=1)
#'
#' @export
#' @importFrom compiler setCompilerOptions cmpfun
#' @importFrom parallel detectCores makeCluster parLapply stopCluster
#'

OOBPredict <-
    function(X, forest, num.cores=0, rank.transform = F, comp.mode = "batch"){
        if (!is.matrix(X)) {
            X <- as.matrix(X)
        }
        if (rank.transform){
            X <- rank.matrix(X)
        }
        n <- nrow(X)

            compiler::setCompilerOptions("optimize"=3)
            comp_errOOB <- compiler::cmpfun(runerrOOB)
            
        comp_errOOB_caller <- function(tree, ...) comp_errOOB(X = X, tree = tree, comp.mode = comp.mode)

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
                    parallel::clusterExport(cl = cl, varlist = c("X", "comp.mode", "comp_errOOB"), envir = environment())
                    Yhats <- parallel::parLapply(cl = cl, forest, fun = comp_errOOB_caller)
                } else {
                    cl <- parallel::makeCluster(spec = num.cores, type = "FORK")
                    Yhats <- parallel::parLapply(cl = cl, forest, fun = comp_errOOB_caller)
                }
                parallel::stopCluster(cl)
        }else{
            #Use just one core.
            Yhats <- lapply(forest, FUN = comp_errOOB_caller)
        }
        num_classes <- ncol(forest[[1]]$ClassProb)
        # Have to make the last entry before this bottom will work.
        scores <- matrix(0,nrow=n, ncol=num_classes)
        oobCounts <- vector(mode = "integer", length = n)
        for(m in 1:f_size){
            oobCounts[forest[[m]]$ind] <- oobCounts[forest[[m]]$ind] + 1L
            for(k in 1:length(forest[[m]]$ind)){
                scores[forest[[m]]$ind[k], Yhats[[m]][k]] <- scores[forest[[m]]$ind[k], Yhats[[m]][k]] + 1
            }
        }
        has.counts <- oobCounts != 0L
        scores[has.counts, ] <- scores[has.counts, ]/oobCounts[has.counts]
        return(scores)
    }
