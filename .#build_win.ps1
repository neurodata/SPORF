# Rscript.exe -e "install.packages('devtools', repos = 'http://cran.us.r-project.org')"
Rscript.exe -e "devtools::document('R')"

R.exe CMD build --resave-data .

# Rscript.exe -e "install.packages('dummies', repos = 'http://cran.us.r-project.org')"
# Rscript.exe -e "install.packages('Rcpp', repos = 'http://cran.us.r-project.org')"
# Rscript.exe -e "install.packages('RcppArmadillo', repos = 'http://cran.us.r-project.org')"
# Rscript.exe -e "install.packages('RcppZiggurat', repos = 'http://cran.us.r-project.org')"
# Rscript.exe -e "install.packages('roxygen2', repos = 'http://cran.us.r-project.org')"
# Rscript.exe -e "install.packages('testthat', repos = 'http://cran.us.r-project.org')"

R.exe CMD check --as-cran --no-manual rerf*.tar.gz
