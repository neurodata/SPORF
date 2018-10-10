source('../rfr_us.R')

# number of trees for forest
numtrees <- 100
# the 'k' of k nearest neighbors
k <- 20


# create a sizeD by m synthetic dataset
X <- as.matrix(iris[,1:4])

# create a similarity matrix using urerf
similarityMatrix <- urerf(X, numtrees, k)
png(file="results/ex10_iris.png")
heatmap(similarityMatrix$similarityMatrix, Rowv=NA, Colv=NA, col=heat.colors(256), symm=TRUE, main="Heatmap of Iris Dataset Similarity Matrix")
dev.off()

