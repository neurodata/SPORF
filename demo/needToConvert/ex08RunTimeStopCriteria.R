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
	sizes <- c(500,1000,5000,10000)

	depths = c(2,4,6,8,10)
	minParents = c(3,5,10,15,20)
	topK <- 3

	results <- data.frame(secondsPerQuery=double(), dataSetSize=double(), stopCriteria=integer(), stringsAsFactors=FALSE)

	q<-1
	for(depth in depths){
		for(sizeD in sizes){
			X <- matrix(sort(runif(m*sizeD)), nrow=sizeD, ncol=m)
			kNN <- as.matrix(dist(X))
			numTest <- 500
			numToTest <- .2*sizeD
			Y <- matrix(sort(runif(m*numTest)), nrow=numTest, ncol=m)

			sM <- urerf(X, numtrees, depth=depth)

			time <- system.time(apprNND <- ann(Y, sM, numToTest), gcFirst = TRUE)[3]
			results[q,] <- c(time/numTest,sizeD ,paste("d",depth,sep=''))
			q <- q+1
		}	
	}


	for(minParent in minParents){
		for(sizeD in sizes){
			X <- matrix(sort(runif(m*sizeD)), nrow=sizeD, ncol=m)
			kNN <- as.matrix(dist(X))
			numTest <- 500
			numToTest <- .2*sizeD
			Y <- matrix(sort(runif(m*numTest)), nrow=numTest, ncol=m)

			sM <- urerf(X, numtrees, minParent)
			time <- system.time(apprNND <- ann(Y, sM, numToTest), gcFirst = TRUE)[3]
			results[q,] <- c(time/numTest,sizeD ,paste("mp",minParent,sep=''))
			q <- q+1
		}
	}

	results[,1] <- as.numeric(results[,1])
	results[,2] <- as.numeric(results[,2])
	results[,3] <- as.factor(results[,3])

	save(results, file="resultsData/ex08_RunTimeStopCriteria.Rdata")
}else{
	load(file="resultsData/ex08_RunTimeStopCriteria.Rdata")
}

png(file="results/ex08_RunTimeStopCriteria.png")
p <- ggplot(aes(x = dataSetSize, y = secondsPerQuery, color=stopCriteria), data = results) + geom_line() + labs(title="RunTime Exploration (Stopping Criteria)\n10-D Line, n=500-10000, d=X, minParent=X, trees=100")
#p <- p + scale_x_reverse()
print(p)
dev.off()

