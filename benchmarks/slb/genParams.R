#! /usr/local/bin/R

## This generates the list of parameters for each run
require(slb)

commitSHA <- "0b999a4382cf0788830d17db68aed5e0eab482e5"
repository <- "uci"
task <- "classification"

type <- read.table("https://raw.githubusercontent.com/j1c/RandomerForest/master/Data/uci/processed/dataset_category.txt")
typeN <- which(type == 'numeric')

n <- names(slb.load.datasets(repositories = repository, task = task))[typeN]

RandMats <- c("RandMatRF", "RandMatBinary", "RandMatContinuous")
Scale01 <- c(TRUE, FALSE)

out <- as.data.frame(expand.grid(commitSHA = commitSHA, repository = repository, task = task, RandMats = RandMats, Scale01 = Scale01, dataset = n))

#write.csv(out, file = "params.csv", row.names = FALSE)
write.csv(out, file = "uci_params.csv", row.names = FALSE)
