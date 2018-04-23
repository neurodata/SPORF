X <- as.matrix(iris[,1:4])
Y <- as.numeric(iris[,5])

library(rerf)
forest <- RerF(X,Y,min.parent =1, max.depth=0, trees=128, seed=sample(1:10000,1),mat.options = list(p = ncol(X), d =ceiling(sqrt(ncol(X))), random.matrix = "rf", rho = 1/ncol(X)), num.cores = 4) 

PackForest(X,Y,forest)

PackPredict(X)
