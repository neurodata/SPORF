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
	numTimes <- 3
	# number of trees for forest
	numtrees <- 100
	# number of dimensions in dataset
	m <- 10 
	# number of samples in dataset
	sizes <- c(100,1000,5000,10000)
	# the 'k' of k nearest neighbors
	k = 10
	numDims <- c(2,4,8,16,32,64)


	topK <- 3

	results <- data.frame(trainingTime=double(), numFeatures=double(), datasetSize=integer())
	q<-1

	time <- NA

	for(sizeD in sizes){
		for(m in numDims){
			X <- matrix(sort(runif(m*sizeD)), nrow=sizeD, ncol=m)
			# create a similarity matrix using urerf
			for(j in 1:numTimes){
				time[j] <- system.time(urerf(X, numtrees, depth=8),gcFirst=TRUE)[3]
			}
			results[q,] <- c(median(time),m,sizeD)
			q <- q + 1
		}
	}

	results[,3] <- as.factor(results[,3])
	save(results, file="resultsData/ex05_UrerfTrainingTime.Rdata")
}else{
	load(file="resultsData/ex05_UrerfTrainingTime.Rdata")
}

png(file="results/ex05_UrerfTrainingTime.png")
p <- ggplot(aes(x = numFeatures, y = trainingTime, colour=datasetSize), data = results) + geom_line() + labs(title="Time To Train Forest\nX-D Line, n=X, depth=8, trees=100")
#p <- p + scale_y_log10()
print(p)
dev.off()
