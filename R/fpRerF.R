#' Packs a forest and saves modified forest to disk for use by PackPredict function
#'
#' Efficiently packs a forest trained with the RF option.  Two intermediate data structures are written to disk, forestPackTempFile.csv and traversalPackTempFile.csv.  The size of these data structures is proportional to a trained forest and training data respectively.  Both data structures are removed at the end of the operation.  The resulting forest is saved as forest.out.  The size of this file is similar to the size of the trained forest.
#'
#' @param X an n by d numeric matrix (preferable) or data frame used to train the forest. 
#' @param Y a numeric vector of size n.  If the Y vector used to train the forest was not of type numeric then a simple call to as.numeric(Y) will suffice as input.
#' @param csvFileName the name of a headerless csv file containing combined data and labels.
#' @param columnWithY is the column in the headerless csv file containing class lables.
#' @param minParent is the size of nodes that will not be split (minParent=1)
#' @param numTreesInForest the number of trees to grow in the forest (numTreesInForest=100)
#' @param numCores is the number of cores to use when training and predicting with the forest (numCores=1)
#' @param numTreeBins the number of bins to store the forest.  Each bin will contain numTreesInForest/numTreeBins trees.  Only used when forestType=="binned*" (numTreeBins= numCores)
#' @param forestType the type of forest to grow: rfBase, rerf, inPlace, inPlaceRerF, binnedBase, binnedBaseRerF (forestType="rerf")
#'
#'
#' @export
#'

fpRerF <-
	function(X=NULL, Y=NULL,csvFileName=NULL, columnWithY=NULL,minParent=1, numTreesInForest=100, numCores=1,numTreeBins=NULL, forestType="rerf"){

		##### Basic Checks
		################################################
		if(numCores < 1){
			stop("at least one core must be used.")
		}
		if(minParent < 1){
			stop("at least one observation must be used in each node.")
		}
		if(numTreesInForest < 1){
			stop("at least one tree must be used.")
		}
		if(is.null(forestType)){
			stop("must pick a forest type: rfBase, rerf, inPlace, inPlaceRerF, binnedBase, binnedBaseRerF")
		}
		if(is.null(numTreeBins)){
			numTreeBins <- numCores
		}

		forest_module <- methods::new(forestPackingRConversion)
		forest_module$setParameterString("forestType", forestType)
		forest_module$setParameterInt("numTreesInForest", numTreesInForest)
		forest_module$setParameterInt("minParent", minParent)
		forest_module$setParameterInt("numCores", numCores)
		forest_module$setParameterInt("useRowMajor",0)


		##### Check X and Y inputs
		################################################
		if(xor(is.null(X), is.null(Y))){
			stop("X and Y must be set or both must be blank.")
		}

		if(!is.null(X)){
			X <- as.matrix(X)
			if(class(X[1,1])=="integer"){
				storage.mode(X) <- "numeric"
			}

			Y <- as.integer(Y)	

			if(nrow(X) != length(Y)){
				stop("number of observations in X is different from Y length.")
			}
			forest_module$growForestGivenX(X,Y)


			##### Check CSV info
			################################################
		}else if(!is.null(csvFileName)){
			if(!file.exists(csvFileName)){
				stop("file does not exist.")
			}
			if(is.null(columnWithY)){
				stop("columnWithY cannot be NULL when using CSV.")
			}
			forest_module$setParameterString("CSVFileName", csvFileName)
			forest_module$setParameterInt("columnWithY", columnWithY);
			forest_module$growForestCSV()
		}else{
			stop("no input provided.")
		}
		#forest_module$printParameters()
		return(forest_module)

	}
