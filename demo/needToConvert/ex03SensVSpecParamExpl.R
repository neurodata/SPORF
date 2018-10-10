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
	numtrees <- 100
	# number of dimensions in dataset
	m <- 10 
	# number of samples in dataset
	sizeD <- 2000
	numToTest <- .2 * sizeD

	depths = c(2,4,6,8,10)
	minParents = c(3,5,10,15,20)
	topK <- 3

	X <- matrix(sort(runif(m*sizeD)), nrow=sizeD, ncol=m)
	kNN <- as.matrix(dist(X))
	Y <- matrix(sort(runif(m*sizeD)), nrow=sizeD, ncol=m)

	NN <- (get.knnx(X,Y,k=topK,algorithm="brute"))$nn.index

	results <- data.frame(sensitivity=double(), specificity=double(), stopCriteria=character(), stringsAsFactors=FALSE)
	q<-1

	for(depth in depths){
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
			results[q,] <- c(numRight/(nrow(Y)*topK), numNegs/((nrow(X)-topK)*nrow(Y)),paste("d",depth,sep=''))
			q <- q+1
		}
	}


	for(minParent in minParents){
		sM <- urerf(X, numtrees, minParent)
		apprNND <- ann(Y, sM, numToTest)

		for(i in topK:numToTest){
			numRight <- 0
			numNegs <- 0
			for(j in 1:nrow(Y)){
				right <- sum(NN[j,] %in% apprNND[j,1:i])
				numRight <- numRight + right
				numNegs <- numNegs + nrow(X)-i-(topK-right)
			}
			results[q,] <- c(numRight/(nrow(Y)*topK), numNegs/((nrow(X)-topK)*nrow(Y)),paste("mp",minParent,sep=''))
			q <- q+1
		}
	}

	results[,1] <- as.numeric(results[,1])
	results[,2] <- as.numeric(results[,2])
	results[,3] <- as.factor(results[,3])

	save(results, file="resultsData/ex03_SensVSpecParam.Rdata")
}else{
	load(file="resultsData/ex03_SensVSpecParam.Rdata")
}

png(file="results/ex03_SensVSpecParam.png")
p <- ggplot(aes(x = specificity, y = sensitivity, color=stopCriteria), data = results) + geom_line() + labs(title="Sensitivity vs Specificity Parameter Exploration (Stopping Criteria)\n10-D Line, n=2000, trees=100")
p <- p + scale_x_reverse()
print(p)
dev.off()


