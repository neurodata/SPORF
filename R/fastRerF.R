#' RerF forest Generator
#'
#' Creates a decision forest based on an input matrix and class vector.  This is the main function in the rerf package.
#'
#' @param CSVFile an n by d numeric matrix (preferable) or data frame. The rows correspond to observations and columns correspond to features.
#' @param Ycolumn an n length vector of class labels.  Class labels must be integer or numeric and be within the range 1 to the number of classes.
#' @param forestType a function that creates the random projection matrix. If NULL and cat.map is NULL, then RandMat is used. If NULL and cat.map is not NULL, then RandMatCat is used, which adjusts the sampling of features when categorical features have been one-of-K encoded. If a custom function is to be used, then it must return a matrix in sparse representation, in which each nonzero is an array of the form (row.index, column.index, value). See RandMat or RandMatCat for details.
#' @param trees parameters in a named list to be used by FUN. If left unchanged,
#' default values will be populated, see \code{\link[rerf]{defaults}} for details.
#' @param min.parent the minimum splittable node size.  A node size < min.parent will be a leaf node. (min.parent = 1)
#' @param minParent the number of trees in the forest. (trees=500)
#'
#' @return forest
#'
#'
#' @export
#'
fastRerF <-
	function(CSVFile, Ycolumn, forestType = "rerf", trees = 100, minParent = 1){

		forestClass <- methods::new(fpForest)
		forestClass$setParamString("forestType", forestType)
		forestClass$setParamInt("numTreesInForest", trees)
		forestClass$setParamInt("minParent", minParent)
		forestClass$setParamString("CSVFileName", CSVFile);
		forestClass$setParamInt("columnWithY", Ycolumn);

		forestClass$growForest();
		return(forestClass)
	}
