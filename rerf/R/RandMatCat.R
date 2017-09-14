#' Create a random projection matrix 
#'
#' Create a random projection matrix using given params. This function differs from RandMat() in that it first samples feature indices from a categorical data matrix/frame that has not been one-of-K encoded.
#' For each categorical feature index sampled, only one of the K feature indices corresponding to the one-of-K encoding is sampled. This method corrects for the fact that features with large K
#' (numbers of categories) will become overrepresented when one-of-K encoded. It also ensures that multiple features in the same one-of-K set are not sampled simultaneously.
#'
#' @param options ?????
#'
#' @return random.matrix ?????
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#' 
#' @export
#' @importFrom RcppZiggurat zrnorm
#'

RandMat <-
  function(options) {
    p <- options[[1L]] # number of dimensions
    d <- options[[2L]] # this determines the number of columns in the projection matrix.
    method <- options[[3L]] # defines the distribution of the random projection matrix
    catMap <- options[[5L]]
    pnum <- catMap[[1L]][1L] - 1L
    #Create the random matrix, a sparse matrix of 1's, -1's, and 0's.
    if (method == "binary") {
      rho <- options[[4L]]
      nnzs <- round(p*d*rho)
      ind <- sort(sample.int((p*d), nnzs, replace = F))
      rw <- ((ind - 1L) %% p) + 1L
      isCat <- rw > pnum
      for (j in (pnum + 1L):p) {
        isj <- rw == j
        rw[isj] <- sample(catMap[[j - pnum]], length(rw[isj]), replace = T)
      }
      return(cbind(rw, floor((ind - 1L) / p) + 1L,
                   sample(c(1L, -1L), nnzs, replace = T)))
    } else if (method == "continuous") {
      rho <- options[[4L]]
      nnzs <- round(p*d*rho)
      ind <- sort(sample.int((p*d), nnzs, replace = F))
      rw <- ((ind - 1L) %% p) + 1L
      isCat <- rw > pnum
      for (j in (pnum + 1L):p) {
        isj <- rw == j
        rw[isj] <- sample(catMap[[j - pnum]], length(rw[isj]), replace = T)
      }
      return(cbind(rw, floor((ind - 1L) / p) + 1L,
                   zrnorm(nnzs)))
    } else if (method == "rf") {
      rw <- sample.int(p, d, replace = F)
      isCat <- rw > pnum
      for (j in (pnum + 1L):p) {
        isj <- rw == j
        rw[isj] <- sample(catMap[[j - pnum]], length(rw[isj]), replace = T)
      }
      return(cbind(rw, 1:d, rep(1L, d)))
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
      isCat <- nz.rows > pnum
      for (j in (pnum + 1L):p) {
        isj <- nz.rows == j
        nz.rows[isj] <- sample(catMap[[j - pnum]], length(nz.rows[isj]), replace = T)
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
      isCat <- nz.rows > pnum
      for (j in (pnum + 1L):p) {
        isj <- nz.rows == j
        nz.rows[isj] <- sample(catMap[[j - pnum]], length(nz.rows[isj]), replace = T)
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
      isCat <- nz.rows > pnum
      for (j in (pnum + 1L):p) {
        isj <- nz.rows == j
        nz.rows[isj] <- sample(catMap[[j - pnum]], length(nz.rows[isj]), replace = T)
      }
      return(cbind(nz.rows, nz.cols, zrnorm(nnz)))
    }
  }
