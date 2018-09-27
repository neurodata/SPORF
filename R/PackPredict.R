#' Compute class predictions for each observation in X
#'
#' Predicts the classification of samples using a trained forest.
#'
#' @param X an n by d numeric matrix (preferable) or data frame. The rows correspond to observations and columns correspond to features of a test set, which should be different from the training set.
#'
#' @param num.cores the number of cores to use while predicting. (num.cores=0)
#'
#' @return predictions an n length vector of prediction class numbers
#'
#' @examples
#' library(rerf)
#' trainIdx <- c(1:40, 51:90, 101:140)
#' X <- as.matrix(iris[,1:4])
#' Y <- as.numeric(iris[,5])
#'
#' paramList <- list(p = ncol(X), d = ceiling(sqrt(ncol(X))))
#'
#' forest <- RerF(X,Y, FUN = RandMatRF, paramList = paramList, rfPack=TRUE, num.cores=1)
#'
#' predictions <- PackPredict(X)
#'
#' @export
#'

PackPredict <-
	function(X, num.cores=1){

		if(file.exists("forest.out")){
			preds <- predictRF(X, num.cores)
		}else{
			print("the file 'forest.out' does not exist")
			return(NA)
		}

		return(preds)
	}
