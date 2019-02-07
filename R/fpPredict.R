#' Packs a forest and saves modified forest to disk for use by PackPredict function
#'
#' Efficiently packs a forest trained with the RF option.  Two intermediate data structures are written to disk, forestPackTempFile.csv and traversalPackTempFile.csv.  The size of these data structures is proportional to a trained forest and training data respectively.  Both data structures are removed at the end of the operation.  The resulting forest is saved as forest.out.  The size of this file is similar to the size of the trained forest.
#'
#' @param forest forest data structure returned from fpRerF function.
#' @param X an n by d numeric matrix (preferable) or data frame used to train the forest. 
#'
#'
#' @export
#'

fpPredict <-
	function(forest=NULL,X=NULL){
		if(class(forest)!="Rcpp_forestPackingRConversion"){
			stop("forest is not of correct type.")
		}

		if(is.null(X)){
			stop("no observations to predict.")
		}

		X <- as.matrix(X)
		if(class(X[1,1])=="integer"){
			storage.mode(X) <- "numeric"
		}

		forest$predict(X)
	}
