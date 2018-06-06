#
# Script for running Regression on RerF and RF
#

rm(list=ls())
# Load in RerF
print("Testing on local branch\n")
devtools::install_local("/Users/Jason/Developer/Research/randomer_forest/R-RerF-fork/")
devtools::load_all("~/Developer/Research/randomer_forest/R-RerF-fork/R/")

# Load in packages
library(mlbench)
library(randomForest)
library(microbenchmark)

# Set up dataset
data(BostonHousing)

X <- cbind(
  BostonHousing$crim,
  BostonHousing$zn,
  BostonHousing$indus,
  BostonHousing$chas,
  BostonHousing$rm,
  BostonHousing$age,
  BostonHousing$dis,
  BostonHousing$rad,
  BostonHousing$tax,
  BostonHousing$ptratio,
  BostonHousing$b,
  BostonHousing$lstat)
Y <- BostonHousing$medv


sample.data <- function(X,Y) {
    N <- length(Y)
    ordering <- sample(N,N)
    split.frac <- 9/10


    # Set aside training data
    bh.Xtrain <- X[ordering[1:ceiling(N*split.frac)],]
    bh.Ytrain <- Y[ordering[1:ceiling(N*split.frac)]]

    # Set aside test dataa
    bh.Xtest <- X[ordering[ceiling(N*split.frac):N],]
    bh.Ytest <- Y[ordering[ceiling(N*split.frac):N]]
    return(list(bh.Xtrain, bh.Ytrain, bh.Xtest, bh.Ytest))
}

compute.mse <- function(predictions, truth) return(mean((predictions - truth)^2))

print.results <- function(predictions, truth, mode) {
    mse <- compute.mse(predictions, truth)
    rmse <- round(sqrt(mse), digits=2)
    print(paste(paste(mode, "MSE:"), mse))
    print(paste(paste(mode, "RMSE:"), rmse))
}

p = dim(X)[2]
d = p
sample.mode = 'rf'
benchmark = FALSE
experiment = TRUE

# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# Benchmarking
# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if (benchmark) {
    print("Starting benchmarking")
    sample = sample.data(X,Y)
    bh.Xtrain = sample[[1]]
    bh.Ytrain = sample[[2]]
    bh.Xtest = sample[[3]]
    bh.Ytest = sample[[4]]
    mbm <- microbenchmark(
        "rerf" = {
            rerf.model <- RerF(bh.Xtrain,
                               bh.Ytrain,
                               num.cores = 1L,
                               task="regression",
                               mat.options=list(p,d,sample.mode),
                               progress = FALSE
                               )
            rerf.predictions <- Predict(bh.Xtest, rerf.model, num.cores = 1L, task='regression')
        },
        "rf" = {
            rf.model <- randomForest(x=data.frame(bh.Xtrain), y=bh.Ytrain, xtest=data.frame(bh.Xtest), ytest=bh.Ytest, replace=TRUE)
        })
    mbm
}


# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# Simple Experiment
# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if (experiment) {
    trials = 10
    rerf.MSE <- rep(0L, trials)
    rf.MSE <- rep(0L, trials)
    print("Starting experiments")
    for (i in 1:trials) {
        if (i %% 1 == 0) print(paste("On epoch:", toString(i)))
        sample = sample.data(X,Y)
        bh.Xtrain = sample[[1]]
        bh.Ytrain = sample[[2]]
        bh.Xtest = sample[[3]]
        bh.Ytest = sample[[4]]
    
    
        set.seed(131)
        rerf.model <- RerF(bh.Xtrain,
                           bh.Ytrain,
                           num.cores = 1L,
                           task="regression",
                           mat.options=list(p,d,sample.mode),
                           progress = FALSE
                           )
        rerf.predictions <- Predict(bh.Xtest, rerf.model, num.cores = 1L, task='regression')
        rerf.MSE[i] <- compute.mse(rerf.predictions, bh.Ytest)
    
    
        # Formulae
        # rf.model <- randomForest(medv ~ . - nox, data=BostonHousing[ordering[ceiling(N*split.frac):N],])
    
        # Non-Formulae
        set.seed(131)
        rf.model <- randomForest(x=data.frame(bh.Xtrain), y=bh.Ytrain, xtest=data.frame(bh.Xtest), ytest=bh.Ytest, replace=TRUE)
        rf.model
        rf.predictions <- rf.model$test$predicted
        rf.MSE[i] <- compute.mse(rf.predictions, bh.Ytest)
    
    }
    print(paste0("RerF average MSE: ", round(mean(rerf.MSE), digits=4)))
    print(paste0("RF average MSE: ", round(mean(rf.MSE), digits=4)))

}
