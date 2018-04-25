#' Compute class predictions for each observation in X
#'
#' Predicts the classification of samples using a trained forest.
#'
#' @param X an n by d numeric matrix (preferable) or data frame. The rows correspond to observations and columns correspond to features of a test set, which should be different from the training set.
#' @param forest a forest trained using the RerF function.
#' @param Y the number of cores to use while training. If NumCores=0 then 1 less than the number of cores reported by the OS are used. (NumCores=0)
#'
#' @return predictions an n length vector of predictions
#'
#' @examples
#' library(rerf)
#' trainIdx <- c(1:40, 51:90, 101:140)
#' X <- as.matrix(iris[,1:4])
#' Y <- as.numeric(iris[,5])
#'
#' @export
#'

PackPredict <-
	function(X){

				if(file.exists("forest.out")){
preds <- predictRF(X)
}

return(preds)
#		print(preds)
	}
