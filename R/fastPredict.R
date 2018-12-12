#' C++ RerF Prediction
#'
#' Creates a decision forest based on an input matrix and class vector.  This is the main function in the rerf package.
#'
#' @param X an n by d numeric matrix (preferable) or data frame. The rows correspond to observations and columns correspond to features.
#' @param forest the forest structure returned from the fastRerF function..
#'
#' @return predictions
#'
#'
#' @export
#'
fastPredict <-
	function(X, forest){
		if(is.null(nrow(X))){
			predictions <- zs$predict(as.numeric(X))
		}else{
			predictions <- NA
			for(i in 1:nrow(X)){
				predictions[i] <- zs$predict(as.numeric(X[i,]))
			}
		}

		return(predictions)
	}
