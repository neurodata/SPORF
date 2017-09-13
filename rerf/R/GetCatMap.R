#' Get categorical feature map. 
#'
#' Returns a list in which the ith element indicates which columns in a one-of-K encoded data matrix correspond to the ith original categorical feature.
#' It reads in a csv file in which the ith line indicates the column indices of the data matrix corresponding to the ith categorical feature.
#' The data matrix should be processed such that the one-of-K encoded categorical features must come after the numeric features. If there are d_cat categorical
#' features, then the csv file should contain d_cat lines. If there are d_num numeric features and the first categorical feature has K1 categories, then the first line in the
#' csv file should read d_num+1,...,d_num+K1. If the second categorical feature has K2 categories, then the second line should read d_num+K1+1,...,d_num+K1+K2.
#' And so on. This function simply reads the lines into a list of integer vectors.
#'
#' @param fileName a string indicating the file name of the categorical map csv file.
#'
#' @return catMap a list in which the ith element indicates which columns in a one-of-K encoded data matrix correspond to the ith original categorical feature.
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