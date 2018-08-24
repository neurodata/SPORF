#' Create a Random Matrix for use when categorical features are present
#'
#' The same as the function RandMat, except that this function is used when a cat.map object is provided (see RerF for details).
#' 
#' @param mat.options the same as that for RandMat, except an additional fifth argument cat.map is taken, which specifies which one-of-K encoded columns in X correspond to the same categorical feature. 
#'
#' @return random.matrix
#'
#' @export
#'
#' @importFrom RcppZiggurat zrnorm
#' @importFrom stats rpois runif

RandMatCat <-
    function(mat.options) {
        p <- mat.options[[1L]] # number of dimensions
        d <- mat.options[[2L]] # this determines the number of columns in the projection matrix.
        method <- mat.options[[3L]] # defines the distribution of the random projection matrix
        catMap <- mat.options[[6L]]
        pnum <- catMap[[1L]][1L] - 1L
        #Create the random matrix, a sparse matrix of 1's, -1's, and 0's.
        if (method == "binary") {
            rho <- mat.options[[4L]]
	    prob <- mat.options[[5L]]
            nnzs <- round(p*d*rho)
            ind <- sort(sample.int((p*d), nnzs, replace = FALSE))
            rw <- ((ind - 1L) %% p) + 1L
            isCat <- rw > pnum
            for (j in (pnum + 1L):p) {
                isj <- rw == j
                rw[isj] <- sample(catMap[[j - pnum]], length(rw[isj]), replace = TRUE)
            }
            return(cbind(rw, floor((ind - 1L) / p) + 1L,
                         sample(c(1L, -1L), nnzs, replace = T, prob = c(prob, 1 - prob))))
        } else if (method == "continuous") {
            rho <- mat.options[[4L]]
            nnzs <- round(p*d*rho)
            ind <- sort(sample.int((p*d), nnzs, replace = FALSE))
            rw <- ((ind - 1L) %% p) + 1L
            isCat <- rw > pnum
            for (j in (pnum + 1L):p) {
                isj <- rw == j
                rw[isj] <- sample(catMap[[j - pnum]], length(rw[isj]), replace = TRUE)
            }
            return(cbind(rw, floor((ind - 1L) / p) + 1L,
                         zrnorm(nnzs)))
        } else if (method == "rf") {
            rw <- sample.int(p, d, replace = FALSE)
            isCat <- rw > pnum
            for (j in (pnum + 1L):p) {
                isj <- rw == j
                rw[isj] <- sample(catMap[[j - pnum]], length(rw[isj]), replace = TRUE)
            }
            return(cbind(rw, 1:d, rep(1L, d)))
        } else if (method == "poisson") {
            lambda <- mat.options[[4L]]
            go <- T
            while (go) {
                nnzPerCol <- stats::rpois(d, lambda)
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
                    nz.rows[start.idx:end.idx] <- sample.int(p, nnzPerCol[i], replace = FALSE)
                    nz.cols[start.idx:end.idx] <- i
                    start.idx <- end.idx + 1L
                }
            }
            isCat <- nz.rows > pnum
            for (j in (pnum + 1L):p) {
                isj <- nz.rows == j
                nz.rows[isj] <- sample(catMap[[j - pnum]], length(nz.rows[isj]), replace = TRUE)
            }
            return(cbind(nz.rows, nz.cols, sample(c(-1L,1L), nnz, replace = TRUE)))
        } else if (method == "frc") {
            nmix <- mat.options[[4L]]
            nnz <- nmix*d
            nz.rows <- integer(nnz)
            nz.cols <- integer(nnz)
            start.idx <- 1L
            for (i in seq.int(d)) {
                end.idx <- start.idx + nmix - 1L
                nz.rows[start.idx:end.idx] <- sample.int(p, nmix, replace = FALSE)
                nz.cols[start.idx:end.idx] <- i
                start.idx <- end.idx + 1L
            }
            isCat <- nz.rows > pnum
            for (j in (pnum + 1L):p) {
                isj <- nz.rows == j
                nz.rows[isj] <- sample(catMap[[j - pnum]], length(nz.rows[isj]), replace = TRUE)
            }
            return(cbind(nz.rows, nz.cols, stats::runif(nnz, -1, 1)))
        } else if (method == "frcn") {
            nmix <- mat.options[[4L]]
            nnz <- nmix*d
            nz.rows <- integer(nnz)
            nz.cols <- integer(nnz)
            start.idx <- 1L
            for (i in seq.int(d)) {
                end.idx <- start.idx + nmix - 1L
                nz.rows[start.idx:end.idx] <- sample.int(p, nmix, replace = FALSE)
                nz.cols[start.idx:end.idx] <- i
                start.idx <- end.idx + 1L
            }
            isCat <- nz.rows > pnum
            for (j in (pnum + 1L):p) {
                isj <- nz.rows == j
                nz.rows[isj] <- sample(catMap[[j - pnum]], length(nz.rows[isj]), replace = TRUE)
            }
            return(cbind(nz.rows, nz.cols, zrnorm(nnz)))
        }
    }
