library(rerf)
x <- as.matrix(read.csv(file="res/higgsData.csv", header=FALSE, sep=","))
#x <- as.matrix(read.csv(file="res/mnist.csv", header=FALSE, sep=","))
y <- as.numeric(x[,1])
x <- as.matrix(x[,2:ncol(x)])

print(paste("there are ", nrow(x), " obs"))

#forest <- RerF(x,y,mat.options = list(p = ncol(x), d = ceiling(sqrt(ncol(x))), random.   matrix = "rf", rho = 1/ncol(x)))


time <- system.time(RerF(x,y,trees=10, mat.options = list(p = ncol(x), d = ceiling(sqrt(ncol(x))), random.matrix = "rf", rho = 1/ncol(x)), num.cores=1), gcFirst = TRUE)[3]

print(paste("it took ", time, " seconds"))

#pred <- Predict(x,forest)

#PackForest(x, y, forest)
