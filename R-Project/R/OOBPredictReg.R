#' Compute out-of-bag predictions
#'
#' Predicts out-of-bag responses for a forest trained with store.oob=TRUE.
#'
#' @param X an n sample by d feature matrix (preferable) or data frame which was used to train the provided forest.
#' @param forest a forest trained using the RerF function, with store.oob=TRUE.
#' @param num.cores the number of cores to use while training. If num.cores=0 then 1 less than the number of cores reported by the OS are used. (num.cores=0)
#'
#' @return predictions a length n vector of predictions in a format similar to the Y vector used to train the forest
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
#' @importFrom parallel detectCores makeCluster parLapply stopCluster
#' @importFrom utils object.size

OOBPredictReg <-
    function(X,
             forest,
             num.cores = 0L){

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

        CompOOBCaller <- function(tree, ...) RunOOBReg(X = X, tree = tree)

        f_size <- length(forest$trees)
        if (num.cores != 1L) {
            if (num.cores == 0L) {
                #Use all but 1 core if num.cores=0.
                num.cores <- parallel::detectCores()-1L
            }
            num.cores <- min(num.cores, f_size)

            gc()
            if ((utils::object.size(forest) > 2e9) |
                (utils::object.size(X) > 2e9) |
                .Platform$OS.type == "windows") {
                cl <- parallel::makeCluster(spec = num.cores, type = "PSOCK")
                parallel::clusterExport(cl = cl, varlist = c("X", "RunOOBReg"), envir = environment())
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
        # Have to make the last entry before this bottom will work.
        predictions <- rep(0, n)
        oobCounts <- vector(mode = "integer", length = n)
        for(m in 1:f_size){
            oobCounts[forest$trees[[m]]$ind] <- oobCounts[forest$trees[[m]]$ind] + 1L
            predictions[forest$trees[[m]]$ind] <- predictions[forest$trees[[m]]$ind] + Yhats[[m]]
        }
        has.counts <- oobCounts != 0L
        predictions[has.counts] <- predictions[has.counts]/oobCounts[has.counts]
        predictions[!has.counts] <- NA

        return(predictions)
    }
