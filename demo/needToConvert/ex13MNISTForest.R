source('../rfr_us.R')

print("Reading MNIST")
X <- as.matrix(read.csv("../data/mnist.csv", header=FALSE))

X <- X[1:10000,2:ncol(X)]
#remove columns with all 0's
X <- X[,which(!apply(X,2,FUN = function(x){all(x == 0)}))]


print("Finished reading MNIST")
print(paste("Data has ", nrow(X), " observations each with ", ncol(X), " features."))

print("starting forest creation.")

depth4Time <- system.time(urerf(X, 10, depth=4), gcFirst=TRUE)[3]
print(paste("it took ", depth4Time, " seconds at depth 4."))

depth8Time <- system.time(urerf(X, 10, depth=8), gcFirst=TRUE)[3]
print(paste("it took ", depth8Time, " seconds at depth 8."))
