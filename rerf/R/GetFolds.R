#' Get K-fold CV partitions
#'
#' Returns a list in which the ith element is an integer vector of the indices of observations in the ith fold
#'
#' @param fileName a csv file in which the ith line indicates indices of observations in the ith fold.
#'
#' @return fold
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#' 
#'
#' @export
#'

GetFolds <-
  function(fileName) {
    con <- file(fileName, "r")
    l <- readLines(con)
    close(con)
    nFolds <- length(l)
    fold <- list()
    for (k in seq.int(nFolds)) {
      fold[[k]] <- as.integer(strsplit(l[k], ",")[[1L]])
    }
    return(fold)
  }