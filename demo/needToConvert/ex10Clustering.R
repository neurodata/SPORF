source('../rfr_us.R')
library(ggplot2)
library(scatterplot3d)
library(MASS)


# number of trees for forest
numtrees <- 100
# number of samples in dataset
sizeD <- 1000
# the 'k' of k nearest neighbors
depth = 8
k = 3


# create a sizeD by m synthetic dataset
X <- swissRoll(sizeD/2, size =1, dim3=T)
X <- as.matrix(X)[,2:4]
#X <- X[order(X[,3], X[,1], X[,2]),]

#X[1:(sizeD/2),] <- X[1:(sizeD/2),]-3
X[(sizeD/2+1):sizeD,1] <- X[(sizeD/2+1):sizeD,1]-1.5

actualClusters <- c(rep(1,sizeD/2), rep(2,sizeD/2))

#create output
png(file="results/ex10_Two3dswissRoll.png")
with(data.frame(X), {
   scatterplot3d(x2, x3, x1,        # x y and z axis
                 color=actualClusters, pch=19, # filled blue circles
                 main="Split 3-D Swiss Roll Actual Two Clusters",
                 xlab="X",
                 ylab="Y",
                 zlab="Z")
})
dev.off()

# create a similarity matrix using urerf
sM <- (urerf(X, numtrees, depth=depth))

clusters <- cluster(sM, 2)

png(file="results/ex10_Two3dswissRollClustered.png")
with(data.frame(X), {
   scatterplot3d(x2, x3, x1,        # x y and z axis
                 color=clusters, pch=19, # filled blue circles
                 main="Split 3-D Swiss Roll Calculated Two Clusters",
                 xlab="X",
                 ylab="Y",
                 zlab="Z")
})
dev.off()



