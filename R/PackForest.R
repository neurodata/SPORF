#' Packs a forest and saves modified forest to disk for use by PackPredict function
#'
#' Efficiently packs a forest trained with the RF option.  Two intermediate data structures are written to disk, forestPackTempFile.csv and traversalPackTempFile.csv.  The size of these data structures is proportional to a trained forest and training data respectively.  Both data structures are removed at the end of the operation.  The resulting forest is saved as forest.out.  The size of this file is similar to the size of the trained forest.
#'
#' @param X an n by d numeric matrix (preferable) or data frame used to train the forest. 
#' @param Y a numeric vector of size n.  If the Y vector used to train the forest was not of type numeric then a simple call to as.numeric(Y) will suffice as input.
#' @param forest a forest trained using the RerF function using the RF option.
#'
 
PackForest <-
	function(X, Y, forest){

		# Make sure forest is of type random forest
		if(!forest$params$mat.options$random.matrix=="rf"){
			stop("The trained forest is not of type random forest")
		}

		# Write forest to CSV
		treeNum <- length(forest$trees)
		if(treeNum < 16 ){
			print("unable to pack forest of size less than 16.")
			return()
		}
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

		print("starting forest write to csv")
		write.table(datWrite, file = "forestPackTempFile.csv",row.names=FALSE, na="",col.names=FALSE, sep=" ", append=FALSE)

		# write training data to csv for stat.  This should become optional
		numberOfFeatures <- ncol(X)
		numberOfTestObservations <- nrow(X)
		listSize <- 2 + numberOfTestObservations*(numberOfFeatures + 1)
		tempTraversal <- vector(mode = "numeric", length = listSize)
		tempTraversal[1] <- numberOfTestObservations
		tempTraversal[2] <- numberOfFeatures

		print("starting traversal write to csv")
		for(j in 0:(numberOfTestObservations-1)){
		tempTraversal[2+j*(numberOfFeatures+1)+1] <- Y[j+1]-1
		tempTraversal[(2+j*(numberOfFeatures+1)+2):(2+(j+1)*(numberOfFeatures+1))] <- X[j+1,]
		}
		write.table(tempTraversal, file = "traversalPackTempFile.csv",row.names=FALSE, na="",col.names=FALSE, sep=" ", append=FALSE)
		
		if (file.exists("forest.out")) file.remove("forest.out")
		# Call C++ code to create and load the forest.
		print("starting packing")
		z <- packForestRCPP()

		print("removing intermediate files")
		if (file.exists("forestPackTempFile.csv")) file.remove("forestPackTempFile.csv")
		if (file.exists("traversalPackTempFile.csv")) file.remove("traversalPackTempFile.csv")
	}
