#!/bin/bash

Rscript -e "install.packages('Rcpp', repos = 'http://cran.us.r-project.org')"
Rscript -e "Rcpp::compileAttributes()"

# Rscript -e "install.packages('devtools', repos = 'http://cran.us.r-project.org')"
# Rscript -e "install.packages('roxygen2', repos = 'http://cran.us.r-project.org')"
Rscript -e "devtools::document('R')"

R CMD build --resave-data .

# Rscript -e "install.packages('dummies', repos = 'http://cran.us.r-project.org')"
# Rscript -e "install.packages('RcppArmadillo', repos = 'http://cran.us.r-project.org')"
# Rscript -e "install.packages('RcppZiggurat', repos = 'http://cran.us.r-project.org')"
# Rscript -e "install.packages('testthat', repos = 'http://cran.us.r-project.org')"

R CMD check --as-cran --no-manual rerf*.tar.gz
