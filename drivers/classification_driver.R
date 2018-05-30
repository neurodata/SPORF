
# Script for running classification on RerF and RF
#


# Load in RerF

# print("Testing on neurodata master\n")
# devtools::install_local("/Users/Jason/Developer/Research/randomer_forest/R-RerF/")
# devtools::load_all("~/Developer/Research/randomer_forest/R-RerF/R/")

print("Testing on local branch\n")
devtools::install_local("/Users/Jason/Developer/Research/randomer_forest/R-RerF-fork/")
devtools::load_all("~/Developer/Research/randomer_forest/R-RerF-fork/R/")


# Load in packages
library(mlbench)
library(randomForest)


# Set up dataset
data(BreastCancer)

X <- cbind(
  BreastCancer$Cl.thickness,
  BreastCancer$Cell.shape,
  BreastCancer$Cell.size,
  BreastCancer$Marg.adhesion,
  BreastCancer$Epith.c.size,
  BreastCancer$Bare.nuclei,
  BreastCancer$Bl.cromatin,
  BreastCancer$Normal.nucleoli,
  BreastCancer$Mitoses
)


Y <- sapply(BreastCancer$Class, function(x) ifelse(x == "benign", 0L, 1L))
Y <- factor(Y)

Y <- Y[rowSums(is.na(X)) == 0]
X <- X[rowSums(is.na(X)) == 0,]

sample.data <- function(X,Y) {
    N <- length(Y)
    ordering <- sample(N,N)
    split.frac <- 9/10


    # Set aside training data
    Xtrain <- X[ordering[1:ceiling(N*split.frac)],]
    Ytrain <- Y[ordering[1:ceiling(N*split.frac)]]

    # Set aside test dataa
    Xtest <- X[ordering[ceiling(N*split.frac):N],]
    Ytest <- Y[ordering[ceiling(N*split.frac):N]]
    return(list(Xtrain, Ytrain, Xtest, Ytest))
}

compute.mse <- function(predictions, truth) return(mean((predictions - truth)^2))
compute.error <- function(predictions, truth) return(mean(predictions != truth))

print.results <- function(predictions, truth, mode) {
    mse <- compute.mse(predictions, truth)
    rmse <- round(sqrt(mse), digits=2)
    print(paste(paste(mode, "MSE:"), mse))
    print(paste(paste(mode, "RMSE:"), rmse))
}

set.seed(131)
p = dim(X)[2]
d = p
sample.mode = 'rf'
trials = 100
benchmark = FALSE
experiment = TRUE

# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# Benchmarking
# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if (benchmark) {
    print("Starting benchmarking")
    sample = sample.data(X,Y)
    bc.Xtrain = sample[[1]]
    bc.Ytrain = sample[[2]]
    bc.Xtest = sample[[3]]
    bc.Ytest = sample[[4]]
    mbm <- microbenchmark(
        "rerf" = {
            rerf.model <- RerF(bc.Xtrain,
                               bc.Ytrain,
                               num.cores = 1L)
            rerf.predictions <- Predict(bc.Xtest, rerf.model, num.cores = 1L)
        },
        "rf" = {
            rf.model <- randomForest(x=data.frame(bc.Xtrain), y=bc.Ytrain, xtest=data.frame(bc.Xtest), ytest=bc.Ytest, replace=T)
        })
    mbm
}



# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# Simple Experiment
# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if (experiment) {
    rerf.ERR <- rep(0L, trials)
    rf.ERR <- rep(0L, trials)
    print("Starting experiments")
    for (i in 1:trials) {
        if (i %% 10 == 0) print(paste("On epoch:", toString(i)))
        sample = sample.data(X,Y)
        bc.Xtrain = sample[[1]]
        bc.Ytrain = sample[[2]]
        bc.Xtest = sample[[3]]
        bc.Ytest = sample[[4]]
    
        # Testing RerF
        set.seed(131)
        rerf.model <- RerF(bc.Xtrain,
                           bc.Ytrain,
                           num.cores = 1L)
        rerf.predictions <- Predict(bc.Xtest, rerf.model, num.cores = 1L)
        rerf.ERR[i] <- compute.error(rerf.predictions, bc.Ytest)
    
    
        # Testing RF
        set.seed(131)
        # Formulae
        # rf.model <- randomForest(medv ~ . - nox, data=BreastCancer[ordering[ceiling(N*split.frac):N],])
        # Non-Formulae
        rf.model <- randomForest(x=data.frame(bc.Xtrain), y=bc.Ytrain, xtest=data.frame(bc.Xtest), ytest=bc.Ytest, replace=T, ntree=100L)
        rf.model
        rf.predictions <- rf.model$test$predicted
        rf.ERR[i] <- compute.error(rf.predictions, bc.Ytest)
    }
    print(paste0("RerF average error: ", round(mean(rerf.ERR), digits=4)))
    print(paste0("RF average error: ", round(mean(rf.ERR), digits=4)))
}


