#!/usr/local/bin/Rscript
require(devtools)
devtools::document('R')
#devtools::check(".", args = c("--no-examples", "--no-tests"))
devtools::check(cran=TRUE, build_args = c("--resave-data"))
devtools::install(".")




