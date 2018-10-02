#' Compute class predictions for each observation in X
#'
#' Predicts the classification of samples using a trained forest.
#'
#' @param X an n by d numeric matrix (preferable) or data frame. The rows correspond to observations and columns correspond to features of a test set, which should be different from the training set.
#' @param forest a forest trained using the RerF function.
#' @param num.cores the number of cores to use while training. If NumCores=0 then 1 less than the number of cores reported by the OS are used. (NumCores=0)
#' @param Xtrain  an n by d numeric matrix (preferable) or data frame. This should be the same data matrix/frame used to train the forest, and is only required if RerF was called with rank.transform = TRUE. (Xtrain=NULL)
#' @param aggregate.output if TRUE then the tree predictions are aggregated via majority vote. Otherwise, the individual tree predictions are returned. (aggregate.output=TRUE)
#' @param output.scores if TRUE then predicted class scores (probabilities) for each observation are returned rather than class labels. (output.scores = FALSE)
#'
#' @return predictions an n length vector of predictions
#'
#' @examples
#' library(rerf)
#' trainIdx <- c(1:40, 51:90, 101:140)
#' X <- as.matrix(iris[,1:4])
#' Y <- as.numeric(iris[,5])
#' forest <- RerF(X[trainIdx, ], Y[trainIdx], num.cores = 1L, rank.transform = TRUE)
#' # Using a set of samples with unknown classification
#' predictions <- Predict(X[-trainIdx, ], forest, num.cores = 1L, Xtrain = X[trainIdx, ])
#' error.rate <- mean(predictions != Y[-trainIdx])
#'
#' @export
#' @importFrom parallel detectCores makeCluster clusterExport parLapply stopCluster
#' @importFrom utils object.size
#'

Predict <-
  function(X, forest, num.cores = 0L, Xtrain = NULL, aggregate.output = TRUE, output.scores = FALSE){
    if (is.data.frame(X)) {
      X <- as.matrix(X)
    }
    if(!is.matrix(X)){
        stop("Observations (X) must be a matrix or data frame.")
    }

    if (forest$params$rank.transform) {
      if (is.null(Xtrain)) {
        ############ error ############
        stop("The model was trained on rank-transformed data. Xtrain must be provided in order to embed Xtest into the rank space")
      } else {
        X <- RankInterpolate(Xtrain, X)
      }
    }

    CompPredictCaller <- function(tree, ...) RunPredict(X=X, tree=tree, task=task)

    f_size <- length(forest$trees)
    if (num.cores != 1L) {
      if (num.cores == 0L) {
        #Use all but 1 core if num.cores=0.
        num.cores=parallel::detectCores()-1L
      }
      num.cores <- min(num.cores, f_size)
      gc()
      if ((utils::object.size(forest) > 2e9) |
          (utils::object.size(X) > 2e9) |
          .Platform$OS.type == "windows") {

        cl <- parallel::makeCluster(spec = num.cores, type = "PSOCK")
        parallel::clusterExport(cl = cl, varlist = c("X", "RunPredict"), envir = environment())
        Yhats <- parallel::parLapply(cl = cl, forest$trees, fun = CompPredictCaller)

      } else {
        cl <- parallel::makeCluster(spec = num.cores, type = "FORK")
        Yhats <- parallel::parLapply(cl = cl, forest$trees, fun = CompPredictCaller)
      }

      parallel::stopCluster(cl)

    } else {
      #Use just one core.
      Yhats <- lapply(forest$trees, FUN = CompPredictCaller)
    }

    if (!aggregate.output) {
      if (task == 'classification')
        predictions <- matrix(forest$labels[unlist(Yhats)], nrow(X), f_size)
      else
        predictions <- matrix(unlist(Yhats), nrow(X), f_size)
    } else {
      if (task == 'classification') {
        num_classes <- ncol(forest$trees[[1L]]$ClassProb)
        predictions <- matrix(0L,nrow=nrow(X), ncol=num_classes)
        for (m in 1L:f_size) {
          for (k in 1L:nrow(X)) {
            predictions[k, Yhats[[m]][k]] <- predictions[k, Yhats[[m]][k]] + 1L
          }
        }
        predictions <- predictions/f_size
        if (!output.scores) {
          if (is.integer(forest$labels)) {
            predictions <- forest$labels[max.col(predictions)]
          } else {
            predictions <- factor(forest$labels[max.col(predictions)], levels = forest$labels)
          }
        }
      } else {
        predictions <-as.matrix( rowMeans(matrix(unlist(Yhats), nrow(X), f_size)))
      }
    }


    return(predictions)
  }
