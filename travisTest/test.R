library(devtools)
#sink("candidate.log")
trees <- 10
numT <- 3
numC <- 3

pvals <- c(.5,2,.5,2, .5,2,.5,1.35)

#The 9th test must be manually changed.  This is the long MNIST.
numTests <- c(rep(numT, 8), 10)
cores <- c(rep(numC, 8), 25)
numTrees <- c(rep(trees, 8), 100)
#This skips the MNIST test.  If you don't want to skip the test #then make skipMNIST FALSE
skipMNIST <- TRUE

trainSets <- c("Orthant_train.csv", "Sparse_parity_train.csv", "Trunk_train.csv", "mnist_train.csv", "calcium-spike_train.csv")
testSets <- c("Orthant_test.csv", "Sparse_parity_test.csv", "Trunk_test.csv", "mnist_test.csv", "calcium-spike_test.csv")

dev_mode(on=FALSE)
install_github("neurodata/R-Rerf@baseline")

pass <- TRUE

testDS <- function(trainSet, testSet, numTests, numTrees, cores, p1){
    X <- read.csv(file(trainSet), header=FALSE)
    Y <- as.numeric(X[,ncol(X)])
    X <- as.matrix(X[,1:(ncol(X)-1)])

    Ymod <- 1 - as.numeric(min(levels(as.factor(Y))))
    Y <- Y+Ymod

    Xtest <- read.csv(file(testSet), header=FALSE)
    Ytest <- as.numeric(Xtest[,ncol(Xtest)])+Ymod
    Xtest <- as.matrix(Xtest[,1:(ncol(Xtest)-1)])

    pass <- TRUE

dev_mode(on=FALSE)
library(rerf)
    ptmtrain_baseline <- NA
    ptmtest_baseline <- NA
    ptmOOB_baseline <- NA
    error_baseline <- NA
    OOBerror_baseline <- NA
    NodeSize_baseline <- NA
    memSize_baseline <- NA

for (i in 1:numTests){
        initMem <- sum(gc(reset=TRUE)[9:10])
        ptm <- proc.time()
        forest<-RerF(X,Y,trees=numTrees, min.parent=1L, max.depth=0, stratify=TRUE, store.oob=TRUE, num.cores=cores, seed = sample(1:10000,1))
        ptmtrain_baseline[i]<- (proc.time() - ptm)[3]
        memSize_baseline[i] <- sum(gc()[9:10]) - initMem
        if (length(forest$trees)!=numTrees){
            print(paste("rfr failed to make desired number of trees.  ", length(forest$trees) , " trees made.  ", numTrees , " trees requested."))
            pass <- FALSE
        }
        temp_size <- 0
        for(z in 1:length(forest$trees)){
            temp_size <- temp_size + length(forest$trees[[z]]$treeMap)
            for(q in 1:length(forest$trees[[z]]$ClassProb[,1])){
                if (sum(forest$trees[[z]]$ClassProb[q,]) == 0){
                    print("A node with 0 samples exists")
                    pass <- FALSE
                }
            }
        }
        NodeSize_baseline[i] <- temp_size/length(forest$trees)
        ptm <- proc.time()
OOBmat_temp <- OOBPredict(X,forest, num.cores=cores)
                        OOBerror_baseline[i] <- mean(OOBmat_temp !=Y)
ptmOOB_baseline[i] <- (proc.time() - ptm)[3]
        ptm <- proc.time()
        error_baseline[i] <- mean(Predict(Xtest,forest,num.cores = cores) != Ytest)
        ptmtest_baseline[i]<- (proc.time() - ptm)[3]
    }

    print("Finished baseline.")
    #####################################################################################################################
detach("package:rerf", unload=TRUE)
    #####################################################################################################################

dev_mode(on=TRUE)
library(rerf)
       ptmtrain_candidate <- NA
    ptmtest_candidate <- NA
ptmOOB_candidate <- NA
    error_candidate <- NA
    OOBerror_candidate <- NA
    NodeSize_candidate <- NA
    memSize_candidate <- NA
    for (i in 1:numTests){
        initMem <- sum(gc(reset=TRUE)[9:10])
        ptm <- proc.time()
        forest<-RerF(X,Y,trees=numTrees, min.parent=1L, max.depth=0, stratify=TRUE, store.oob=TRUE, num.cores=cores, seed = sample(1:10000,1))
        ptmtrain_candidate[i]<- (proc.time() - ptm)[3]
        memSize_candidate[i] <- sum(gc()[9:10]) - initMem
        if (length(forest$trees)!=numTrees){
            print(paste("rfr failed to make desired number of trees.  ", length(forest$trees) , " trees made.  ", numTrees , " trees requested."))
            pass <- FALSE
        }
        temp_size <- 0
        for(z in 1:length(forest$trees)){
            temp_size <- temp_size + length(forest$trees[[z]]$treeMap)
            for(q in 1:length(forest$trees[[z]]$ClassProb[,1])){
                if (sum(forest$trees[[z]]$ClassProb[q,]) == 0){
                    print("A node with 0 samples exists")
                    pass <- FALSE
                }
            }
        }
        NodeSize_candidate[i] <- temp_size/length(forest$trees)
        ptm <- proc.time()
OOBmat_temp <- OOBPredict(X,forest, num.cores=cores)
                        OOBerror_candidate[i] <- mean(OOBmat_temp != Y)
ptmOOB_candidate[i] <- (proc.time() - ptm)[3]
        ptm <- proc.time()
        error_candidate[i] <- mean(Predict(Xtest,forest,num.cores = cores) != Ytest)
        ptmtest_candidate[i]<- (proc.time() - ptm)[3]
    }
detach("package:rerf", unload=TRUE)


    NodeSizeRatio = median(NodeSize_candidate)/median(NodeSize_baseline)
    memSizeRatio = median(memSize_baseline)/median(memSize_candidate)
    OOBerrorRatio = median(OOBerror_baseline)/median(OOBerror_candidate)
    errorRatio = median(error_baseline)/median(error_candidate)
    ptmtrainRatio = median(ptmtrain_baseline)/median(ptmtrain_candidate)
    ptmtestRatio = median(ptmtest_baseline)/median(ptmtest_candidate)
ptmOOBRatio = median(ptmOOB_baseline)/median(ptmOOB_candidate)

    print("")
    print(paste(trainSet, "pmod= ", p1))
    wt <- wilcox.test(NodeSize_baseline, NodeSize_candidate)$p.value
    print(paste("Node Size, pval(2sided)= ", wt, ", ratio(C/B)= ", NodeSizeRatio, ", pass=", wt>.001))
    if(!is.nan(wt)){
        if(wt<.001){
            pass <- FALSE
        }
    }

    wt <- wilcox.test(OOBerror_baseline, OOBerror_candidate)$p.value
    print(paste("OOBerror, pval(2sided)= ", wt, ", ratio(B/C)= ", OOBerrorRatio, ", pass=", wt>.001))
    if(!is.nan(wt)){
        if(wt<.001){
            pass <- FALSE
        }
    }

    wt <- wilcox.test(error_baseline, error_candidate)$p.value
    print(paste("Error, pval(2sided)= ", wt, ", ratio(B/C)= ", errorRatio, ", pass=", wt>.001))
    if(!is.nan(wt)){
        if(wt<.001){
            pass <- FALSE
        }
    }

    wt <- wilcox.test(ptmtrain_baseline, ptmtrain_candidate, alternative="l")$p.value
    print(paste("Train Time, pval(l)= ", wt, ", ratio(B/C)= ", ptmtrainRatio, ", pass=", wt>.001))
    if(!is.nan(wt)){
        if(wt<.001){
            pass <- FALSE
        }
    }

    wt <- wilcox.test(ptmtest_baseline, ptmtest_candidate, alternative="l")$p.value
    print(paste("Test Time, pval(l)= ", wt, ", ratio(B/C)= ", ptmtestRatio, ", pass=", wt>.001))
    if(!is.nan(wt)){
        if(wt<.001){
            pass <- FALSE
        }
    }

wt <- wilcox.test(ptmOOB_baseline, ptmOOB_candidate, alternative="l")$p.value
    print(paste("OOB Time, pval(l)= ", wt, ", ratio(B/C)= ", ptmOOBRatio, ", pass=", wt>.001))
    if(!is.nan(wt)){
        if(wt<.001){
            pass <- FALSE
        }
    }

    print(paste("mem ratio for training (B/C): ", memSizeRatio))
    flush.console()
    return(pass)
}


lh <- NA
for (z in 1:(2*length(trainSets)-skipMNIST)){
    gc()
    lh[z] <- testDS(trainSets[ceiling(z/2)], testSets[ceiling(z/2)], numTests[z], numTrees[z], cores[z], pvals[z])
    if (lh[z]==FALSE){
        pass <- FALSE
        print(paste(trainSets[ceiling(z/2)], " failed"))
    }
}

if(pass){
    print("All tests passed.")
}else{
    print("Candidate failed.")
}
if(!pass){
    quit(status=1)
}
#sink()

