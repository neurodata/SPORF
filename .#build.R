#!/usr/local/bin/Rscript
require(devtools)
devtools::document('R')
#devtools::check(".", args = c("--no-examples", "--no-tests"))
devtools::build(".", args = c("--resave-data"))
devtools::check(".", args = c("--as-cran"))
devtools::install(".")




