rmat <-
function(options) {
  p <- options[[1]] # number of dimensions
  d <- options[[2]] # this determines the number of columns in the projection matrix.
  method <- options[[3]] # defines the distribution of the random projection matrix
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
    nnz.cum <- cumsum(nnzPerCol)
    nz.rows <- integer(nnz.cum[d])
    nz.cols <- integer(nnz.cum[d])
    if (nnzPerCol[1L] != 0L) {
      nz.rows[1:nnzPerCol[1L]] <- sample.int(p, nnzPerCol[1L], replace = F)
      nz.cols[1:nnzPerCol[1L]] <- 1L
    }
    for (i in 2:d) {
      if (nnzPerCol[i] != 0L) {
        nz.rows[(nnz.cum[i - 1L] + 1L):nnz.cum[i]] <- sample.int(p, nnzPerCol[i], replace = F)
        nz.cols[(nnz.cum[i - 1L] + 1L):nnz.cum[i]] <- i
      }
    }
    # nz.rows <- c(unlist(sapply(nnzPerCol, function(x) sort(sample.int(p, x, replace = F)))))
    # nz.cols <- c(unlist(mapply(rep, 1:d, nnzPerCol)))
    return(cbind(nz.rows, nz.cols, sample(c(-1L,1L), nnz.cum[d], replace = T)))
  } else if (method == "frc") {
    nmix <- options[[4L]]
    nnz.cum <- seq.int(nmix, nmix*d, nmix)
    nz.rows <- integer(nnz.cum[d])
    nz.cols <- integer(nnz.cum[d])
    nz.rows[1:nmix] <- sample.int(p, nmix, replace = F)
    nz.cols[1:nmix] <- 1L
    for (i in 2:d) {
      nz.rows[(nnz.cum[i - 1L] + 1L):nnz.cum[i]] <- sample.int(p, nmix, replace = F)
      nz.cols[(nnz.cum[i - 1L] + 1L):nnz.cum[i]] <- i
    }
    return(cbind(nz.rows, nz.cols, runif(nnz.cum[d], -1, 1)))
  } else if (method == "frcn") {
    nmix <- options[[4L]]
    nnz.cum <- seq.int(nmix, nmix*d, nmix)
    nz.rows <- integer(nnz.cum[d])
    nz.cols <- integer(nnz.cum[d])
    nz.rows[1:nmix] <- sample.int(p, nmix, replace = F)
    nz.cols[1:nmix] <- 1L
    for (i in 2:d) {
      nz.rows[(nnz.cum[i - 1L] + 1L):nnz.cum[i]] <- sample.int(p, nmix, replace = F)
      nz.cols[(nnz.cum[i - 1L] + 1L):nnz.cum[i]] <- i
    }
    return(cbind(nz.rows, nz.cols, zrnorm(nnz.cum[d])))
  }
}
