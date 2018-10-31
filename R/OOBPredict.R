#' Compute out-of-bag predictions
#'
#' Computes out-of-bag class predictions for a forest trained with store.oob=TRUE.
#'
#' @param X an n sample by d feature matrix (preferable) or data frame which was used to train the provided forest.
#' @param forest a forest trained using the RerF function, with store.oob=TRUE.
#' @param num.cores the number of cores to use while training. If num.cores=0 then 1 less than the number of cores reported by the OS are used. (num.cores=0)
#' @param Xtrain  an n by d numeric matrix (preferable) or data frame. This should be the same data matrix/frame used to train the forest, and is only required if RerF was called with rank.transform = TRUE. (Xtrain=NULL)
#' @param output.scores if TRUE then predicted class scores (probabilities) for each observation are returned rather than class labels. (output.scores = FALSE)
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

OOBPredict <- function(X, forest, num.cores = 0L, Xtrain = NULL, output.scores = FALSE) {
  predictions <- Predict(X = X, forest = forest, OOB = TRUE, num.cores = num.cores, 
    Xtrain = Xtrain, output.scores = output.scores)

  mes1 <- c("NA's generated in the OOB predictions because %d/%d observations were never out-of-bag.")
  mes2 <- "\nA fix for this is to re-train the forest using more trees."

  if (!output.scores && any(is.na(predictions))) {
    num <- sum(is.na(predictions))
    warning(paste(sprintf(mes1, num, nrow(X)), mes2), call. = TRUE)
  }

  if (output.scores && any(is.na(predictions))) {
    num <- sum(is.na(predictions[, 1]))
    warning(paste(sprintf(mes1, num, nrow(X)), mes2), call. = TRUE)
  }

  return(predictions)
}



