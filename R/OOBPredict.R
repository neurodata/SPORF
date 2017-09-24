#' Compute out-of-bag predictions
#'
#' Computes out-of-bag class predictions for a forest trained with store.oob=TRUE.
#'
#' @param X an n sample by d feature matrix (preferable) or data frame which was used to train the provided forest.
#' @param forest a forest trained using the RerF function, with store.oob=TRUE.
#' @param num.cores the number of cores to use while training. If num.cores=0 then 1 less than the number of cores reported by the OS are used. (num.cores=0)
#' @param output.scores if TRUE then predicted class scores (probabilities) for each observation are returned rather than class labels. (output.scores = FALSE)
#'
#' @return predictions a length n vector of predictions in a format similar to the Y vector used to train the forest
#'
#' @author James Browne and Tyler Tomita, jbrowne6@jhu.edu and ttomita2@jhmi.edu 
#'
#' @examples
#' library(rerf)
#' X <- as.matrix(iris[,1:4])
#' Y <- iris[[5L]]
#' forest <- RerF(X, Y, store.oob=TRUE, num.cores = 1L)
#' predictions <- OOBPredict(X, forest, num.cores = 1L)
#' oob.error <- mean(predictions != Y)
#'
#' @export
#' @importFrom compiler setCompilerOptions cmpfun
#' @importFrom parallel detectCores makeCluster parLapply stopCluster

OOBPredict <-
    function(X, 
             forest, 
             num.cores = 0L, 
             output.scores = FALSE){

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

        CompOOBCaller <- function(tree, ...) RunOOB(X = X, tree = tree)

        f_size <- length(forest$trees)
        if (num.cores != 1L) {
            if (num.cores == 0L) {
                #Use all but 1 core if num.cores=0.
                num.cores <- parallel::detectCores()-1L
            }
            num.cores <- min(num.cores, f_size)

            gc()
            if ((object.size(forest) > 2e9) || (object.size(X) > 2e9) || forest$params$rotate) {
                cl <- parallel::makeCluster(spec = num.cores, type = "PSOCK")
                parallel::clusterExport(cl = cl, varlist = c("X", "RunOOB"), envir = environment())
                Yhats <- parallel::parLapply(cl = cl, forest$trees, fun = CompOOBCaller)
            } else {
                cl <- parallel::makeCluster(spec = num.cores, type = "FORK")
                Yhats <- parallel::parLapply(cl = cl, forest$trees, fun = CompOOBCaller)
            }

           # num.cores <- min(num.cores, f_size)
           # cl <- parallel::makeCluster(spec = num.cores, type = "PSOCK")
      #      Yhats <- parallel::parLapply(cl = cl, forest$trees, fun = CompOOBCaller)
            parallel::stopCluster(cl)
        } else {
            #Use just one core.
            Yhats <- lapply(forest$trees, FUN = CompOOBCaller)
        }
        num_classes <- length(forest$labels)
        # Have to make the last entry before this bottom will work.
        predictions <- matrix(0,nrow=n, ncol=num_classes)
        oobCounts <- vector(mode = "integer", length = n)
        for(m in 1:f_size){
            oobCounts[forest$trees[[m]]$ind] <- oobCounts[forest$trees[[m]]$ind] + 1L
            for(k in 1:length(forest$trees[[m]]$ind)){
                predictions[forest$trees[[m]]$ind[k], Yhats[[m]][k]] <- predictions[forest$trees[[m]]$ind[k], Yhats[[m]][k]] + 1L
            }
        }
        has.counts <- oobCounts != 0L
        predictions[has.counts, ] <- predictions[has.counts, ]/oobCounts[has.counts]
        if (!output.scores) {
            if (is.integer(forest$labels)) {
                predictions <- forest$labels[max.col(predictions)]
            } else {
                predictions <- factor(forest$labels[max.col(predictions)], levels = forest$labels)
            }
        }
        return(predictions)
    }
