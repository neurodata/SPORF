source('../rfr_us.R')
library(ggplot2)


X <- as.matrix(iris[,1:4])

sM <- urerf(X)

cluss1 <- cluster(sM,3,"average")
print("confusion matrix for 'average' cluster")
print(table(cluss1, iris$Species))

cluss2 <- cluster(sM,3,"mcquitty")
print("confusion matrix for 'mcquitty' cluster")
print(table(cluss2, iris$Species))

cluss3 <- cluster(sM,3,"kmeans")
print("confusion matrix for 'kmeans' cluster")
print(table(cluss3, iris$Species))

cluss4 <- cluster(sM,3,"medoids")
print("confusion matrix for 'medoids' cluster")
print(table(cluss4, iris$Species))
