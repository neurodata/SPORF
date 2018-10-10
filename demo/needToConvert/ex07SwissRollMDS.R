source('../rfr_us.R')
library(ggplot2)
library(scatterplot3d)
library(MASS)


# number of trees for forest
numtrees <- 100
# number of samples in dataset
sizeD <- 2000
# the 'k' of k nearest neighbors
depth = 8
k = 3


# create a sizeD by m synthetic dataset
X <- swissRoll(sizeD/2, size =1, dim3=T)
X <- as.matrix(X)[,2:4]
X <- X[order(X[,3], X[,1], X[,2]),]

#create output
png(file="results/ex07_3dswissRoll.png")
with(data.frame(X), {
   scatterplot3d(x2, x3, x1,        # x y and z axis
                 color=rainbow(sizeD), pch=19, # filled blue circles
                 main="3-D Swiss Roll",
                 xlab="X",
                 ylab="Y",
                 zlab="Z")
})
dev.off()

# create a similarity matrix using urerf
similarityMatrix <- 1-(urerf(X, numtrees, depth=depth))$similarityMatrix

fit <- cmdscale(as.dist(similarityMatrix), eig=FALSE, k=2)

png(file="results/ex07_3dswissRollMDSMetric.png")
plot(fit, col=rainbow(sizeD), main="Metric MDS")
dev.off()


fit <- isoMDS(similarityMatrix, k=2)

png(file="results/ex07_3dswissRollMDSNonmetric.png")
plot(fit$points, col=rainbow(sizeD), main="Nonmetric MDS")
dev.off()


# number of trees for forest
numtrees <- 100
# number of samples in dataset
sizeD <- 2000
# the 'k' of k nearest neighbors
k <- 3
depth <- 8
#line dimensions
m <- 10


# create a sizeD by m synthetic dataset
	X <- matrix(sort(runif(m*sizeD)), nrow=sizeD, ncol=m)
X <- X[order(X[,3], X[,1], X[,2]),]

#create output
png(file="results/ex07_3d_10dLine.png")
with(data.frame(X), {
   scatterplot3d(X1, X2, X3,        # x y and z axis
                 color=rainbow(sizeD), pch=19, # filled blue circles
                 main="3-D of 10-D Line",
                 xlab="X",
                 ylab="Y",
                 zlab="Z")
})
dev.off()

# create a similarity matrix using urerf
similarityMatrix <- 1-(urerf(X, numtrees, depth=depth))$similarityMatrix

fit <- cmdscale(as.dist(similarityMatrix), eig=FALSE, k=2)

png(file="results/ex07_10dto2dLineMDSMetric.png")
plot(fit, col=rainbow(sizeD), main="Metric MDS")
dev.off()


fit <- isoMDS(similarityMatrix, k=2)

png(file="results/ex07_10dto2dLineMDSNonmetric.png")
plot(fit$points, col=rainbow(sizeD), main="Nonmetric MDS")
dev.off()


