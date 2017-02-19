test_easy <- function(){
source("rf_function.R");

X <- read.csv("data/easy_gen.csv", header = TRUE);
Y <- X[,3]+1; #don't forget to start at 1 instead of 0.
X<- X[,1:2];

resC <- build_tree(X,Y,2,10);
return(resC);
}

#testing RandomForest on wdbc data
test_wdbc <- function(){
source("rf_function.R");

X <- read.csv("data/wdbc.data", header=FALSE);
Y <- as.numeric(X[,2]);
X <- X[,3:ncol(X)];

resD <- build_tree(X,Y,5, 10);
return(resD);
}

#testing RandomForest on Trunk data.
test_trunk <- function(){
source("rf_function.R");
X<- read.csv("data/Trunk/Trunk_Xtrain.dat", header=FALSE)
Y<- read.csv("data/Trunk/Trunk_Ytrain.dat", header=FALSE) +1

resD <- build_tree(X,Y, 5, 10);
return(resD);
}

#test RandomerForest on Tyler's easy dataset.
test_easy_R <- function(){
source("rfr_function.R");

X <- read.csv("data/easy_gen.csv", header = TRUE);
Y <- X[,3]+1; #don't forget to start at 1 instead of 0.
X<- X[,1:2];

resC <- build_tree(X,Y,2,10);
return(resC);
}

#testing RandomerForest on wdbc data
test_wdbc_R <- function(){
source("rfr_function.R");

X <- read.csv("data/wdbc.data", header=FALSE);
Y <- as.numeric(X[,2]);
X <- X[,3:ncol(X)];

resD <- build_tree(X,Y,5, 10);
return(resD);
}

#testing RandomerForest on Trunk data.
test_trunk_R <- function(){
source("rfr_function.R");
X<- read.csv("data/Trunk/Trunk_Xtrain.dat", header=FALSE)
Y<- read.csv("data/Trunk/Trunk_Ytrain.dat", header=FALSE) +1

resD <- build_tree(X,Y, 5, 10);
return(resD);
}
