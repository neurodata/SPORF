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

PackForest <-
	function(X, Y, forest){

		# Make sure forest is of type random forest
		if(!forest$params$mat.options$random.matrix=="rf"){
			stop("The trained forest is not of type random forest")
		}

		# Write forest to CSV
		treeNum <- length(forest$trees)
		treeSizes <- NA
		datWrite <- c(treeNum, ncol(forest$trees[[1]]$ClassProb))

		for(i in 1:treeNum){
			classProbs <- NA
			features <- NA
			for(z in 1:length(forest$trees[[i]]$ClassProb[,1])){
				classProbs[z] <- which.max(forest$trees[[i]]$ClassProb[z,])-1
			}
			features <- forest$trees[[i]]$matAstore[seq(1, length(forest$trees[[i]]$matAstore),2)]-1
			treeSizes[i] <- length(forest$trees[[i]]$treeMap)
			datWrite <- c(datWrite, treeSizes[i], forest$trees[[i]]$treeMap, forest$trees[[i]]$CutPoint, classProbs, features)
		}

		write.table(datWrite, file = "forestPackTempFile.csv",row.names=FALSE, na="",col.names=FALSE, sep=" ", append=FALSE)

		# write training data to csv for stat.  This should become optional
		numberOfFeatures <- ncol(X)
		numberOfTestObservations <- nrow(X)
		tempTraversal <- c(numberOfTestObservations, numberOfFeatures)

		#write.table(c(numberOfTestObservations, numberOfFeatures), file = "traversal.csv", row.names=FALSE, na="",col.names=FALSE, sep=" ",append=FALSE)
		for(j in 1:numberOfTestObservations){
			#write.table(c(Y[j]-1, X[j,]), file = "traversalPackTempFile.csv",row.names=FALSE, na="",col.names=FALSE, sep=" ", append=TRUE)
		tempTraversal <-	c(tempTraversal, Y[j]-1, X[j,])
		}
		write.table(tempTraversal, file = "traversalPackTempFile.csv",row.names=FALSE, na="",col.names=FALSE, sep=" ", append=FALSE)

		# Call C++ code to create and load the forest.
		z <- testFun()
		print(paste("z = ", z))

		if (file.exists("forestPackTempFile.csv")) file.remove("forestPackTempFile.csv")
		if (file.exists("traversalPackTempFile.csv")) file.remove("traversalPackTempFile.csv")

#		if(file.exists("forest.out")){
#preds <- predictRF(X)
#}

#		preds
	}
