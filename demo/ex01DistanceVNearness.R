library(ggplot2)
library(rerf)

# number of trees for forest
numtrees <- 100
# number of dimensions in dataset
m <- 10
# number of samples in dataset
sizeD <- 1000
# the 'k' of k nearest neighbors
depth=8
k = 3

	# create a sizeD by m synthetic dataset
	X <- matrix(sort(runif(m*sizeD)), nrow=sizeD, ncol=m)
	AkNN <- matrix(0, nrow=sizeD, ncol=sizeD)

	# find the actual euclidean distance between all samples of the synthetic dataset
	for(z in 1:sizeD){
		AkNN[z,] <- sqrt(rowSums(sweep(X,2,X[z,])^2))
	}

	# create a similarity matrix using Lumberjack
	sM <- UnsupervisedLumberjack(X, trees=numtrees, max.depth=depth)
	nnzPts <- which(sM$similarityMatrix != 0 & sM$similarityMatrix != 1)

	#create output
	ssd <- data.frame(Distance = AkNN[nnzPts], Nearness = sM$similarityMatrix[nnzPts])

	png(file="ex01_10dLineDistVNearAll.png")
	p <- ggplot(aes(x = Nearness, y = Distance), data = ssd) + geom_point() + labs(title="Distance vs Similarity of All Points to All Other Points\n10-D Line, n=1000, depth=8, trees=100\n(0 Similarity Omitted)")
	p <- p + scale_y_log10() 
	p <- p + xlab("Similarity") + ylab("Euclidean Distance")
	print(p)
	dev.off()

	png(file="ex01_10dLineDistVNear3Subset.png")
	nnzPts <- which(sM$similarityMatrix[,499:501] != 0 & sM$similarityMatrix[,499:501] != 1)
	ssd <- data.frame(Distance = AkNN[,499:501][nnzPts], Nearness = sM$similarityMatrix[,499:501][nnzPts])
	groupLabels <- c(rep("499",sizeD), rep("500", sizeD), rep("501",sizeD ))[nnzPts]
	ssd[["Sample"]] <- groupLabels
	p <- ggplot(aes(x = Nearness, y = Distance, color = Sample), data = ssd) + geom_point()+ labs(title="Distancech vs Similarity of points 499-501 to all other points\n10-D Line, n=1000, depth=8, trees=100\nThree Samples (0 Similarity omitted)")+ geom_jitter()
	p <- p + scale_y_log10() 
	print(p)
	dev.off()

	png(file="ex01_10dLineDistVNear1Subset.png")
	nnzPts <- which(sM$similarityMatrix[,500] != 1)
	ssd <- data.frame(Distance = AkNN[,500][nnzPts], Nearness = sM$similarityMatrix[,500][nnzPts])
	groupLabels <- (rep("500", sizeD))[nnzPts]
	ssd[["Sample"]] <- groupLabels
	p<-ggplot(aes(x = Nearness, y = Distance), data = ssd) + geom_point()+ labs(title="Distance vs Similarity of One Observation to All Others\n10-D Line, n=1000, depth=8, trees=100\nThree Samples (0 Similarity Retained)")+ geom_jitter()
	p <- p + scale_y_log10() 
	print(p)
	dev.off()
	
