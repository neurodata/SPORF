#! /usr/local/bin/R

## This generates the list of parameters for each run
require(slb)

commitSHA <- "0b999a4382cf0788830d17db68aed5e0eab482e5"
repository <- "pmlb"
task <- "classification"

n <- names(slb.load.datasets(repositories = repository, task = task))

RandMats <- c("RandMatRF", "RandMatBinary", "RandMatContinuous")
Scale01 <- c(TRUE, FALSE)

out <- as.data.frame(expand.grid(commitSHA = commitSHA, repository = repository, task = task, RandMats = RandMats, Scale01 = Scale01, dataset = n))

head(out)

write.csv(out, file = "params.csv", row.names = FALSE)
