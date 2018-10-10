source('../rfr_us.R')
library(ggplot2)
library(FNN)

# number of trees for forest
numtrees <- 100
# number of dimensions in dataset
m <- 100
# number of samples in dataset
sizeD <- 1000
# the 'k' of k nearest neighbors
k = 20
depth = 8

# create a sizeD by m synthetic dataset
X <- matrix(sort(runif(m*sizeD)), nrow=sizeD, ncol=m)
numY <- 100
Y <- matrix(sort(runif(m*numY)), nrow=numY, ncol=m)
Y[3,] <- X[3,]

# find the actual euclidean distance between all samples of the synthetic dataset
# create a similarity matrix using urerf
sM <- urerf(X, numtrees, depth=depth)


ann2 <- function(X, urerfS){
	X <- sweep(X, 2, urerfS$colMin, "-")
	X <-	 sweep(X, 2, urerfS$colMax, "/")

	numTrees <- length(urerfS$forest)

	recursiveTreeTraversal <- function(currNode, testCase, treeNum){
		if(urerfS$forest[[treeNum]]$Children[currNode]==0L){
			return(urerfS$forest[[treeNum]]$ALeaf[currNode])
		}

		s<-length(urerfS$forest[[treeNum]]$matA[[currNode]])/2
		rotX <- apply(X[testCase,urerfS$forest[[treeNum]]$matA[[currNode]][(1:s)*2-1], drop=FALSE], 1, function(x) sum(urerfS$forest[[treeNum]]$matA[[currNode]][(1:s)*2]*x))
		moveLeft <- rotX<=urerfS$forest[[treeNum]]$CutPoint[currNode]

		if(moveLeft){
			recursiveTreeTraversal( urerfS$forest[[treeNum]]$Children[currNode,1L], testCase, treeNum)
		}else{
			recursiveTreeTraversal( urerfS$forest[[treeNum]]$Children[currNode,2L], testCase, treeNum)
		}
	}

	#	output <- NA
	output <- matrix(0,nrow=nrow(X), ncol=urerfS$trainSize)
	for(i in 1:nrow(X)){
		matches <- numeric(urerfS$trainSize) 
		for(j in 1:numTrees){
			elementsInNode <- recursiveTreeTraversal(1L, i, j)
			if(length(elementsInNode[[1]])==0){
				print("found one")
			}
			matches[elementsInNode[[1]]] <- matches[elementsInNode[[1]]] + 1
		}
		#output[i,] <- order(matches,decreasing=TRUE)
		output[i,] <- matches
	}
	output
}

NN <- get.knnx(X,Y,k=nrow(X),algorithm="brute")
for(i in 1:nrow(Y)){
	NN$nn.dist[i,] <- NN$nn.dist[i,order(NN$nn.index[i,])]
}
AkNN <- NN$nn.dist

nearnessNew <- ann2(Y, sM)/numtrees

nnzPts <- which(nearnessNew != 0 & nearnessNew != 1 )

#create output
ssd <- data.frame(Distance = AkNN[nnzPts], Nearness = nearnessNew[nnzPts])

png(file="results/ex09_DistanceVNearnessNewPointAllPlusOneTraining.png")
p <- ggplot(aes(x = Nearness, y = Distance), data = ssd) + geom_point() + labs(title="Distance vs Similarity of All New Points to All Training Points\n10-D Line, n=1000, depth=8, trees=100\n(0 Similarity Omitted)")
p <- p + scale_y_log10() 
p <- p + xlab("Similarity") + ylab("Euclidean Distance (Log10)")
print(p)
dev.off()


nearnessAllNew <- nearnessNew[-3,]
nnzPts <- which(nearnessAllNew != 0 & nearnessAllNew != 1 )
#create output
ssd <- data.frame(Distance = AkNN[-3,][nnzPts], Nearness = nearnessAllNew[nnzPts])
png(file="results/ex09_DistanceVNearnessNewPointAllNoTraining.png")
p <- ggplot(aes(x = Nearness, y = Distance), data = ssd) + geom_point() + labs(title="Distance vs Similarity of All New Points to All Training Points (Exact Match Omitted)\n10-D Line, n=1000, depth=8, trees=100\n(0 Similarity Omitted)")
p <- p + scale_y_log10() 
p <- p + xlab("Similarity") + ylab("Euclidean Distance (Log10)")
print(p)
dev.off()


nnzPts <- which(nearnessNew[49:51,] != 0 & nearnessNew[49:51,] != 1 )
ssd <- data.frame(Distance = AkNN[49:51,][nnzPts], Nearness = nearnessNew[49:51,][nnzPts])
groupLabels <- c(rep("49",nrow(X)), rep("50", nrow(X)), rep("51",nrow(X) ))[nnzPts]
ssd[["Sample"]] <- groupLabels
png(file="results/ex09_DistanceVNearnessNewPointSubset.png")
p <- ggplot(aes(x = Nearness, y = Distance, color = Sample), data = ssd) + geom_point()+ labs(title="Distancec vs Similarity of points 49-51 to all other points in Training Set\n10-D Line, n=1000, d=10, k=10, trees=100\nThree Samples (0 Similarity omitted)") + geom_jitter()
p <- p + scale_y_log10() 
p <- p + xlab("Similarity") + ylab("Euclidean Distance (Log10)")
print(p)
dev.off()


nnzPts <- which(AkNN[3:4,] != 0)
ssd <- data.frame(Distance = AkNN[3:4,][nnzPts], Nearness = nearnessNew[3:4,][nnzPts])
groupLabels <- c(rep("trained", nrow(X)), rep("new", nrow(X)))[nnzPts]
ssd[["Sample"]] <- groupLabels
png(file="results/ex09_DistanceVNearnessNewPoint1Subset.png")
p<-ggplot(aes(x = Nearness, y = Distance, color = Sample), data = ssd) + geom_point()+ labs(title="Distance vs Similarity of point 500 from all other points\n10-D Line, n=1000, d=10, k=10, trees=100\nThree Samples (0 Similarity Retained)", legend="test") + geom_jitter()
p <- p + scale_y_log10() 
p <- p + xlab("Similarity") + ylab("Euclidean Distance (Log10)")
print(p)
dev.off()


ssd <- data.frame(Distance = AkNN[4,][1:length(AkNN[4,])], Nearness = nearnessNew[4,][1:length(AkNN[4,])])
groupLabels <- c(rep("new", nrow(X)))
png(file="results/ex09_DistanceVNearnessNewPoint1SubsetNew.png")
p<-ggplot(aes(x = Nearness, y = Distance), data = ssd) + geom_point()+ labs(title="Distance vs Similarity of One New Point to All Training Points\n10-D Line, n=1000, depth=8, trees=100\n(0 Similarity Retained)") + geom_jitter()
p <- p + scale_y_log10() 
p <- p + xlab("Similarity") + ylab("Euclidean Distance (Log10)")
print(p)
dev.off()

