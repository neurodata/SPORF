source('../rfr_us.R')
library(ggplot2)
library(FNN)

LoadAndPrintOnly <- FALSE
args <- commandArgs(trailingOnly = TRUE)
if(length(args)==1){
	if(args[1] == "print"){
		LoadAndPrintOnly <- TRUE
	}
}

if(!LoadAndPrintOnly){
	# number of trees for forest
	trees <- c(10,50,100,500,1000,2000)
	# number of dimensions in dataset
	m <- 10 
	# number of samples in dataset
	sizeD <- 2000
	numToTest <- .2 * sizeD
	depth = 8
	topK <- 3

	X <- matrix(sort(runif(m*sizeD)), nrow=sizeD, ncol=m)
	kNN <- as.matrix(dist(X))
	Y <- matrix(sort(runif(m*sizeD)), nrow=sizeD, ncol=m)

	NN <- (get.knnx(X,Y,k=topK,algorithm="brute"))$nn.index

	results <- data.frame(sensitivity=double(), specificity=double(), trees=integer())
	q<-1

	for(numtrees in trees){
		sM <- urerf(X, numtrees, depth=depth)

		apprNND <- ann(Y, sM, numToTest)

		for(i in topK:numToTest){
			numRight <- 0
			numNegs <- 0
			for(j in 1:nrow(Y)){
				right <- sum(NN[j,] %in% apprNND[j,1:i])
				numRight <- numRight + right
				numNegs <- numNegs + nrow(X)-i-(topK-right)
			}
			results[q,] <- c(numRight/(nrow(Y)*topK), numNegs/((nrow(X)-topK)*nrow(Y)),numtrees)
			q <- q+1
		}
	}

	results[,3] <- as.factor(results[,3])

	save(results, file="resultsData/ex06_SensVSpecTrees.Rdata")
}else{
	load(file="resultsData/ex06_SensVSpecTrees.Rdata")
}

png(file="results/ex06_SensVSpecTrees.png")
p <- ggplot(aes(x = specificity, y = sensitivity, color=trees), data = results) + geom_line() + labs(title="Sensitivity vs Specificity Parameter Exploration (Trees)\n10-D Line, n=2000, depth=8, trees=X")
p <- p + scale_x_reverse()
print(p)
dev.off()


