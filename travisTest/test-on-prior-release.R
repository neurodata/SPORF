require(devtools)
require(testthat)

context("Iterate over multiple datasets/releases and multiple runs to test runtimes & accuracy")


trees <- 10
numT <- 5 # number of tests
numC <- 3 # number of cores to use

# Last test is usually skipped (MNIST data -- long)
numTests <- c(rep(numT, 4), 10)
cores <- c(rep(numC, 4), 25)
numTrees <- c(rep(trees, 4), 100)

path <- "travisTest/"
trainSets <- c(
  "Orthant_train.csv", "Sparse_parity_train.csv", "Trunk_train.csv",
  "calcium-spike_train.csv", "mnist_train.csv"
)
testSets <- c(
  "Orthant_test.csv", "Sparse_parity_test.csv", "Trunk_test.csv", "calcium-spike_test.csv",
  "mnist_test.csv"
)
trainSets <- sapply(trainSets, function(x) paste(path, x, sep = ""))
testSets <- sapply(testSets, function(x) paste(path, x, sep = ""))


test_skips <- c(FALSE, FALSE, FALSE, FALSE, TRUE) # skip MNIST data


#' Gets memory used
#' @param x boolean passed to gc reset
#'
getMemUsed <- function(x = FALSE) {
  tmp <- gc(reset = x)
  sum(tmp[, ncol(tmp) - 1])
}





#' Creates forests for a dataset and records their accuracy and times
#' @param trainSet
#' @param testSet
#' @param numTests num. times it is run
#' @param numTrees num. trees to use
#' @param cores num. of cores used
#' @return list of timings and accuracies, where len == numTests
testDS <- function(trainSet, testSet, numTests, numTrees, cores) {
  X <- read.csv(trainSet, header = FALSE)
  Y <- as.numeric(X[, ncol(X)])
  X <- as.matrix(X[, 1:(ncol(X) - 1)])

  Ymod <- 1 - as.numeric(min(levels(as.factor(Y))))
  Y <- Y + Ymod

  Xtest <- read.csv(testSet, header = FALSE)
  Ytest <- as.numeric(Xtest[, ncol(Xtest)]) + Ymod
  Xtest <- as.matrix(Xtest[, 1:(ncol(Xtest) - 1)])

  # initializing return values
  ptmtrain <- NA
  ptmtest <- NA
  ptmOOB <- NA
  error <- NA
  OOBerror <- NA
  NodeSize <- NA
  memSize <- NA

  for (i in 1:numTests) {
    # garbage collection, last two columns gives memory used since last call to gc
    initMem <- getMemUsed(TRUE)

    ptm <- proc.time() # start timing

    # create forest
    forest <- RerF(X, Y,
      trees = numTrees, min.parent = 1L, max.depth = 0, stratify = TRUE,
      store.oob = TRUE, num.cores = cores, seed = sample(1:10000, 1)
    )

    ptmtrain[i] <- (proc.time() - ptm)[3] # time taken

    memSize[i] <- getMemUsed() - initMem # memory used

    temp_size <- 0
    for (z in 1:length(forest$trees)) {
      temp_size <- temp_size + length(forest$trees[[z]]$treeMap)
    }

    NodeSize[i] <- temp_size / length(forest$trees)

    # time to do the OOB prediction
    ptm <- proc.time()
    OOBmat_temp <- suppressWarnings(OOBPredict(X, forest, num.cores = cores))
    OOBerror[i] <- mean(OOBmat_temp != Y, na.rm = TRUE)
    ptmOOB[i] <- (proc.time() - ptm)[3]

    # time to do prediction
    ptm <- proc.time()
    error[i] <- mean(Predict(Xtest, forest, num.cores = cores) != Ytest)
    ptmtest[i] <- (proc.time() - ptm)[3]
  }

  ret_vals = list(
    ptmtrain = ptmtrain, ptmtest = ptmtest, ptmOOB = ptmOOB, error = error,
    OOBerror = OOBerror, NodeSize = NodeSize, memSize = memSize
  )
  return(ret_vals)
}


testDSs <- function(trainSets, testSets, numTests, numTrees, cores, test_skips) {
  results <- list()
  for (i in which(!test_skips)) {
    gc() # run gc to keep timing pure?
    ret_vals <- testDS(trainSets[i], testSets[i], numTests[i], numTrees[i], cores[i])
    results[[i]] <- ret_vals
  }
  return(results)
}

# first running on candidate
devtools::dev_mode(on = FALSE)
library("rerf")

# this is the results from the candidate release
results_candidate <- testDSs(trainSets, testSets, numTests, numTrees, cores, test_skips)

detach("package:rerf", unload = TRUE)
print("Finished candidate.")

# dev mode will let us load in a different version
devtools::dev_mode(on = TRUE)
# Checking out latest version on CRAN
install.packages("rerf", repos = 'http://cran.us.r-project.org')
library("rerf")

# this is the results from the stable version
results_baseline <- testDSs(trainSets, testSets, numTests, numTrees, cores, test_skips)

detach("package:rerf", unload = TRUE)
devtools::dev_mode(on = FALSE)
print("Finished baseline.")


# Compare performance (timings & accuracy) between prior 'release' and current
# 'tested' version Using dev mode on/off Record timed tests & accuracy for each
# version Compare the distributions between the different versions to ensure that
# they are from the 'same' distribution (wilcox test p value >= .001)

# test if results are from same distribution
dist_same <- function(A, B) {
  wt <- wilcox.test(A, B)$p.value
  if (wt < 0.001) {
    return(FALSE)
  }
  return(TRUE)
}

test_that("test different parameters and performance to make sure they are similar", {
  for (i in which(!test_skips)) {
    for (param in 1:length(results_candidate[[1]])) {
      print(names(results_candidate[[i]])[param])
      expect_true(dist_same(results_candidate[[i]][[param]], results_baseline[[i]][[param]]))
    }
  }
})

print("Test against prior release finished")
