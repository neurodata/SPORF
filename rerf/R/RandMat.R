#' Create a Random Matrix 
#'
#' Create a random matrix using given params.
#'
#' @param options ?????
#'
#' @return random.matrix ?????
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#' 
#'
#' @importFrom RcppZiggurat zrnorm
#'

RandMat <-
function(options) {
  p <- options[[1L]] # number of dimensions
  d <- options[[2L]] # this determines the number of columns in the projection matrix.
  method <- options[[3L]] # defines the distribution of the random projection matrix
  #Create the random matrix, a sparse matrix of 1's, -1's, and 0's.
  if (method == "binary") {
    rho <- options[[4L]]
    nnzs <- round(p*d*rho)
    ind <- sort(sample.int((p*d), nnzs, replace = F))
    return(cbind(((ind - 1L) %% p) + 1L, floor((ind - 1L) / p) + 1L,
                 sample(c(1L, -1L), nnzs, replace = T)))
  } else if (method == "continuous") {
    rho <- options[[4L]]
    nnzs <- round(p*d*rho)
    ind <- sort(sample.int((p*d), nnzs, replace = F))
    return(cbind(((ind - 1L) %% p) + 1L, floor((ind - 1L) / p) + 1L,
                 zrnorm(nnzs)))
  } else if (method == "rf") {
    return(cbind(sample.int(p, d, replace = F), 1:d, rep(1L, d)))
  } else if (method == "poisson") {
    lambda <- options[[4L]]
    go <- T
    while (go) {
      nnzPerCol <- rpois(d, lambda)
      go <- !any(nnzPerCol)
    }
    nnzPerCol[nnzPerCol > p] <- p
    nnz <- sum(nnzPerCol)
    nz.rows <- integer(nnz)
    nz.cols <- integer(nnz)
    start.idx <- 1L
    for (i in seq.int(d)) {
      if (nnzPerCol[i] != 0L) {
        end.idx <- start.idx + nnzPerCol[i] - 1L
        nz.rows[start.idx:end.idx] <- sample.int(p, nnzPerCol[i], replace = F)
        nz.cols[start.idx:end.idx] <- i
        start.idx <- end.idx + 1L
      }
    }
    return(cbind(nz.rows, nz.cols, sample(c(-1L,1L), nnz, replace = T)))
  } else if (method == "frc") {
    nmix <- options[[4L]]
    nnz <- nmix*d
    nz.rows <- integer(nnz)
    nz.cols <- integer(nnz)
    start.idx <- 1L
    for (i in seq.int(d)) {
      end.idx <- start.idx + nmix - 1L
      nz.rows[start.idx:end.idx] <- sample.int(p, nmix, replace = F)
      nz.cols[start.idx:end.idx] <- i
      start.idx <- end.idx + 1L
    }
    return(cbind(nz.rows, nz.cols, runif(nnz, -1, 1)))
  } else if (method == "frcn") {
    nmix <- options[[4L]]
    nnz <- nmix*d
    nz.rows <- integer(nnz)
    nz.cols <- integer(nnz)
    start.idx <- 1L
    for (i in seq.int(d)) {
      end.idx <- start.idx + nmix - 1L
      nz.rows[start.idx:end.idx] <- sample.int(p, nmix, replace = F)
      nz.cols[start.idx:end.idx] <- i
      start.idx <- end.idx + 1L
    }
    return(cbind(nz.rows, nz.cols, zrnorm(nnz)))
  }
}
