#' Get K-fold CV partitions
#'
#' Reads in a csv file in which the kth line lists the indices of observations in the kth fold
#'
#' @param fileName ?????
#'
#' @return catMap ?????
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#' 
#'
#' @export
#'

GetCatMap <-
  function(fileName) {
    con <- file(fileName, "r")
    l <- readLines(con)
    close(con)
    pcat <- length(l)
    catMap <- list()
    for (j in seq.int(pcat)) {
      catMap[[j]] <- as.integer(strsplit(l[j], ",")[[1L]])
    }
    return(catMap)
  }