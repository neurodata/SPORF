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
sizes <- c(100,1000,5000,10000)
# the 'k' of k nearest neighbors
k = 10
numDims <- c(2,4,8,16,32,64)

topK <- 3

results <- data.frame(secondsPerQuery=double(), numFeatures=double(), datasetSize=integer())
	q<-1

for(sizeD in sizes){
	for(m in numDims){
X <- matrix(sort(runif(m*sizeD)), nrow=sizeD, ncol=m)
# create a similarity matrix using urerf
sM <- urerf(X, numtrees, depth=8)
testSize <- 500
numToTest <- .2 * sizeD
Y <- matrix(sort(runif(m*testSize)), nrow=testSize, ncol=m)

time <- system.time(apprNND <- ann(Y, sM, numToTest), gcFirst = TRUE)
results[q,] <- c(time[3]/testSize,m,sizeD)
q <- q + 1
	}
}

results[,3] <- as.factor(results[,3])
save(results, file="resultsData/ex04_ANNRunTime.Rdata")
}else{
load(file="resultsData/ex04_ANNRunTime.Rdata")
}

png(file="results/ex04_ANNRunTime.png")
p <- ggplot(aes(x = numFeatures, y = secondsPerQuery, colour=datasetSize), data = results) + geom_line() + labs(title="Time To Find A-NN\nX-D Line, n=100-20000, depth=8, trees=100")
#p <- p + scale_y_log10()
print(p)
dev.off()
