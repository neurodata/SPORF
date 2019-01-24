#! /usr/local/bin/R

## This generates the list of parameters for each run
require(slb)

repository <- "pmlb"
task <- "classification"

n <- names(slb.load.datasets(repositories = repository, task = task))

RandMats <- c("RandMatRF", "RandMatBinary", "RandMatContinuous")
Scale01 <- c(TRUE, FALSE)

out <- as.data.frame(expand.grid(repository = repository, task = task, RandMats = RandMats, Scale01 = Scale01, dataset = n))

head(out)

write.csv(out, file = "params.csv", row.names = FALSE)
