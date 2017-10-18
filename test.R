X <- read.csv("wdbc.data", header=F)
Y <- X[,2]
X <- X[,3:ncol(X)]
library(rerf)
forest <- RerF(X,Y,min.parent =1, max.depth=0, mat.options = list(p = ifelse(is.null(cat.map), ncol(X), length(cat.map)), d = ceiling(sqrt(ncol(X))), random.matrix = "rf", rho = ifelse(is.null(cat.map), 1/ncol(X), 1/length(cat.map))))
z <- NA
for(i in 1:100){
z[i]<-sum(forest$trees[[i]]$treeMap<1)-sum(forest$trees[[i]]$treeMap>1)
}
z
