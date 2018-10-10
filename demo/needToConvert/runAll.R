experimentsToRun <- list.files(".",pattern="*.R", full.names=TRUE)
lapply(experimentsToRun[experimentsToRun!="./runAll.R"],source)
