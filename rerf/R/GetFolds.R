#' Get K-fold CV partitions
#'
#' Reads in a csv file in which the kth line lists the indices of observations in the kth fold
#'
#' @param fileName ?????
#'
#' @return fold ?????
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