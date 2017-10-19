#X <- read.csv("../rold2/data/wdbc.data",header=F)
#Y <- as.numeric(as.numeric(X[,2]))
#X <- X[3:32]
#X <- matrix(runif(60000), ncol=30, nrow=2000)
#Y <- sample(1:2, 2000, TRUE)
library(rerf)
#forest <- RerF(X,Y,min.parent =1,bagging=0, max.depth=0, trees=1, store.ns=TRUE, mat.options = list(p = ncol(X), d = ceiling(sqrt(ncol(X))), random.matrix = "rf", rho = 1/ncol(X)))


X<- matrix(runif(30*5000)+.10, ncol=30)
X<- rbind(X,matrix(runif(30*5000), ncol=30))

Y<-as.numeric(c(rep(2,5000),rep(1,5000)))

forest <- RerF(X,Y,min.parent =1,bagging=0, max.depth=0, trees=1, store.ns=TRUE, mat.options = list(p = ncol(X), d = ncol(X), random.matrix = "rf", rho = 1/ncol(X)))
forest$trees

X1<- matrix(runif(30*5000)+.10, ncol=30)
X1 <- rbind(X1,matrix(runif(30*5000), ncol=30))
Y1<-as.numeric(c(rep(2,5000),rep(1,5000)))

depth <- PredictD(X1,forest)
df <- data.frame(x=depth)

ggplot(df,aes(x)) + stat_ecdf(geom = "step", pad = FALSE)

