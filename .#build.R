#!/usr/local/bin/Rscript
require(devtools)
devtools::document('R')
devtools::check(".")
devtools::install(".")


