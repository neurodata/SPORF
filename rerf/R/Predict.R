#' Compute class predictions for each observation in X
#'
#' Predicts the classification of samples using a trained forest.
#'
#' @param X an n by d numeric matrix (preferable) or data frame. The rows correspond to observations and columns correspond to features of a test set, which should be different from the training set.
#' @param forest a forest trained using the rerf function.
#' @param num.cores the number of cores to use while training. If NumCores=0 then 1 less than the number of cores reported by the OS are used. (NumCores=0)
#' @param rank.transform TRUE indicates that the forest was built on rank-transformed data. (rank.transform=FALSE)
#' @param Xtrain  an n by d numeric matrix (preferable) or data frame. This should be the same data matrix/frame used to train the forest, and is only required if rank.transform is TRUE. (Xtrain=NULL)
#' @param aggregate.output if TRUE then the tree predictions are aggregated via majority vote. Otherwise, the individual tree predictions are returned. (aggregate.output=TRUE)
#'
#' @return predictions
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#' 
#' @examples
#' library(rerf)
#' X <- as.matrix(iris[,1:4])
#' Y <- as.numeric(iris[,5])
#' trainedForest <- rerf(X, Y, num.cores=1)
#' # Using a set of samples with unknown classification 
#' Predict(X, trainedForest, num.cores=1)
#'
#' @export
#' @importFrom compiler setCompilerOptions cmpfun
#' @importFrom parallel detectCores makeCluster clusterExport parLapply stopCluster
#'

Predict <-
    function(X, forest, num.cores=0, rank.transform = F, Xtrain = NULL, aggregate.output = T){
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

            compiler::setCompilerOptions("optimize"=3)
            comp_predict <- compiler::cmpfun(runpredict)

        comp_predict_caller <- function(tree, ...) comp_predict(X=X, tree=tree)

        f_size <- length(forest)
        if (num.cores != 1L) {
                if (num.cores == 0L) {
                    #Use all but 1 core if num.cores=0.
                    num.cores=parallel::detectCores()-1L
                }
                #Start mclapply with num.cores Cores.
                num.cores <- min(num.cores, f_size)
                gc()
                if ((object.size(forest) > 2e9) | (object.size(X) > 2e9)) {
                    cl <- parallel::makeCluster(spec = num.cores, type = "PSOCK")
                    parallel::clusterExport(cl = cl, varlist = c("X", "comp.mode", "comp_predict"), envir = environment())
                    Yhats <- parallel::parLapply(cl = cl, forest, fun = comp_predict_caller)
                } else {
                    cl <- parallel::makeCluster(spec = num.cores, type = "FORK")
                    Yhats <- parallel::parLapply(cl = cl, forest, fun = comp_predict_caller)
                }
                parallel::stopCluster(cl)

        } else {
            #Use just one core.
            Yhats <- lapply(forest, FUN = comp_predict_caller)
        }

        if (!aggregate.output) {
            predictions <- matrix(unlist(Yhats), nrow(X), f_size)
        } else {
            num_classes <- ncol(forest[[1L]]$ClassProb)
            predictions <- matrix(0,nrow=nrow(X), ncol=num_classes)
            for (m in 1:f_size) {
                for (k in 1:nrow(X)) {
                    predictions[k, Yhats[[m]][k]] <- predictions[k, Yhats[[m]][k]] + 1
                }
            }
            predictions <- predictions/f_size
        }
        return(predictions)
    }
