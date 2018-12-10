#' Create a Random Matrix: Binary
#'
#'
#' @param p the number of dimensions.
#' @param d the number of desired columns in the projection matrix.
#' @param sparsity a real number in \eqn{(0,1)} that specifies the distribution of non-zero elements in the random matrix.
#' @param prob a probability \eqn{\in (0,1)} used for sampling from
#' \eqn{{-1,1}} where \code{prob = 0} will only sample -1 and \code{prob = 1} will only sample 1.
#' @param catMap a list specifying specifies which one-of-K encoded columns in X correspond to the same categorical feature.
#' @param ... used to handle superfluous arguments passed in using paramList.
#'
#' @return A random matrix to use in running \code{\link{RerF}}.
#'
#' @export
#'
#' @examples
#'
#' p <- 8
#' d <- 3
#' sparsity <- 0.25
#' prob <- 0.5
#' set.seed(4)
#' (a <- RandMatBinary(p, d, sparsity, prob))
RandMatBinary <- function(p, d, sparsity, prob, catMap = NULL, ...) {
  nnzs <- round(p * d * sparsity)
  ind <- sort(sample.int((p * d), nnzs, replace = FALSE))

  ## Determine if categorical variables need to be taken into
  ## consideration
  if (is.null(catMap)) {
    randomMatrix <- cbind(((ind - 1L) %% p) + 1L, floor((ind - 1L) / p) +
      1L, sample(c(1L, -1L), nnzs, replace = TRUE, prob = c(
      prob,
      1 - prob
    )))
  } else {
    pnum <- catMap[[1L]][1L] - 1L
    rw <- ((ind - 1L) %% p) + 1L
    isCat <- rw > pnum
    for (j in (pnum + 1L):p) {
      isj <- rw == j
      rw[isj] <- sample(catMap[[j - pnum]], length(rw[isj]), replace = TRUE)
    }
    randomMatrix <- cbind(rw, floor((ind - 1L) / p) + 1L, sample(c(
      1L,
      -1L
    ), nnzs, replace = TRUE, prob = c(prob, 1 - prob)), deparse.level = 0)
  }
  return(randomMatrix)
}


#' Create a Random Matrix: Continuous
#'
#'
#' @param p the number of dimensions.
#' @param d the number of desired columns in the projection matrix.
#' @param sparsity a real number in \eqn{(0,1)} that specifies the distribution of non-zero elements in the random matrix.
#' @param catMap a list specifying specifies which one-of-K encoded columns in X correspond to the same categorical feature.
#' @param ... used to handle superfluous arguments passed in using paramList.
#'
#' @return A random matrix to use in running \code{\link{RerF}}.
#'
#' @importFrom RcppZiggurat zrnorm
#'
#' @export
#'
#' @examples
#'
#' p <- 8
#' d <- 3
#' sparsity <- 0.25
#' set.seed(4)
#' (a <- RandMatContinuous(p, d, sparsity))
RandMatContinuous <- function(p, d, sparsity, catMap = NULL, ...) {
  nnzs <- round(p * d * sparsity)
  ind <- sort(sample.int((p * d), nnzs, replace = FALSE))

  if (is.null(catMap)) {
    randomMatrix <- cbind(((ind - 1L) %% p) + 1L, floor((ind - 1L) / p) +
      1L, zrnorm(nnzs))
  } else {
    pnum <- catMap[[1L]][1L] - 1L
    rw <- ((ind - 1L) %% p) + 1L
    isCat <- rw > pnum
    for (j in (pnum + 1L):p) {
      isj <- rw == j
      rw[isj] <- sample(catMap[[j - pnum]], length(rw[isj]), replace = TRUE)
    }
    randomMatrix <- cbind(rw, floor((ind - 1L) / p) + 1L, zrnorm(nnzs),
      deparse.level = 0
    )
  }
  return(randomMatrix)
}


#' Create a Random Matrix: Random Forest (RF)
#'
#'
#' @param p the number of dimensions.
#' @param d the number of desired columns in the projection matrix.
#' @param catMap a list specifying specifies which one-of-K encoded columns in X correspond to the same categorical feature.
#' @param ... used to handle superfluous arguments passed in using paramList.
#'
#' @return A random matrix to use in running \code{\link{RerF}}.
#'
#' @export
#'
#' @examples
#'
#' p <- 8
#' d <- 3
#' paramList <- list(p = p, d = d)
#' set.seed(4)
#' (a <- do.call(RandMatRF, paramList))
RandMatRF <- function(p, d, catMap = NULL, ...) {
  if (d > p) {
    stop("ERROR: parameter d is greater than the number of dimensions p.")
  }

  if (is.null(catMap)) {
    randomMatrix <- cbind(sample.int(p, d, replace = FALSE), 1:d, rep(1L, d))
  } else {
    pnum <- catMap[[1L]][1L] - 1L
    rw <- sample.int(p, d, replace = FALSE)
    isCat <- rw > pnum
    for (j in (pnum + 1L):p) {
      isj <- rw == j
      rw[isj] <- sample(catMap[[j - pnum]], length(rw[isj]), replace = TRUE)
    }
    randomMatrix <- cbind(rw, 1:d, rep(1L, d), deparse.level = 0)
  }
  return(randomMatrix)
}


#' Create a Random Matrix: Poisson
#'
#' Samples a binary projection matrix where sparsity is distributed
#' \eqn{Poisson(\lambda)}.
#'
#' @param p the number of dimensions.
#' @param d the number of desired columns in the projection matrix.
#' @param lambda passed to the \code{\link[stats]{rpois}} function for generation of non-zero elements in the random matrix.
#' @param catMap a list specifying specifies which one-of-K encoded columns in X correspond to the same categorical feature.
#' @param ... used to handle superfluous arguments passed in using paramList.
#'
#' @return A random matrix to use in running \code{\link{RerF}}.
#'
#' @importFrom stats rpois
#'
#' @export
#'
#' @examples
#'
#' p <- 8
#' d <- 8
#' lambda <- 0.5
#' paramList <- list(p = p, d = d, lambda = lambda)
#' set.seed(8)
#' (a <- do.call(RandMatPoisson, paramList))
RandMatPoisson <- function(p, d, lambda, catMap = NULL, ...) {
  if (lambda <= 0) {
    stop("ERROR: Wrong parameter for Poisson, make sure lambda > 0.")
  }

  nnzPerCol <- stats::rpois(d, lambda)
  while (!any(nnzPerCol)) {
    nnzPerCol <- stats::rpois(d, lambda)
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

  if (is.null(catMap)) {
    randomMatrix <- cbind(nz.rows, nz.cols, sample(c(-1L, 1L), nnz,
      replace = TRUE
    ))
  } else {
    pnum <- catMap[[1L]][1L] - 1L
    isCat <- nz.rows > pnum
    for (j in (pnum + 1L):p) {
      isj <- nz.rows == j
      nz.rows[isj] <- sample(catMap[[j - pnum]], length(nz.rows[isj]),
        replace = TRUE
      )
    }
    randomMatrix <- cbind(nz.rows, nz.cols, sample(c(-1L, 1L), nnz,
      replace = TRUE
    ), deparse.level = 0)
  }
  return(randomMatrix)
}


#' Create a Random Matrix: FRC
#'
#'
#' @param p the number of dimensions.
#' @param d the number of desired columns in the projection matrix.
#' @param nmix mupliplier to \code{d} to specify the number of non-zeros.
#' @param catMap a list specifying specifies which one-of-K encoded columns in X correspond to the same categorical feature.
#' @param ... used to handle superfluous arguments passed in using paramList.
#'
#' @return A random matrix to use in running \code{\link{RerF}}.
#'
#' @importFrom stats runif
#'
#' @export
#'
#' @examples
#'
#' p <- 8
#' d <- 8
#' nmix <- 5
#' paramList <- list(p = p, d = d, nmix = nmix)
#' set.seed(4)
#' (a <- do.call(RandMatFRC, paramList))
RandMatFRC <- function(p, d, nmix, catMap = NULL, ...) {
  if (nmix > p) {
    stop("ERROR: parameter nmix is greater than the number of dimensions p.")
  }

  nnz <- nmix * d
  nz.rows <- integer(nnz)
  nz.cols <- integer(nnz)
  start.idx <- 1L
  for (i in seq.int(d)) {
    end.idx <- start.idx + nmix - 1L
    nz.rows[start.idx:end.idx] <- sample.int(p, nmix, replace = FALSE)
    nz.cols[start.idx:end.idx] <- i
    start.idx <- end.idx + 1L
  }

  if (is.null(catMap)) {
    randomMatrix <- cbind(nz.rows, nz.cols, runif(nnz, -1, 1))
  } else {
    pnum <- catMap[[1L]][1L] - 1L
    isCat <- nz.rows > pnum
    for (j in (pnum + 1L):p) {
      isj <- nz.rows == j
      nz.rows[isj] <- sample(catMap[[j - pnum]], length(nz.rows[isj]),
        replace = TRUE
      )
    }
    randomMatrix <- cbind(nz.rows, nz.cols, runif(nnz, -1, 1),
      deparse.level = 0
    )
  }
  return(randomMatrix)
}


#' Create a Random Matrix: FRCN
#'
#'
#' @param p the number of dimensions.
#' @param d the number of desired columns in the projection matrix.
#' @param nmix mupliplier to \code{d} to specify the number of non-zeros.
#' @param catMap a list specifying specifies which one-of-K encoded columns in X correspond to the same categorical feature.
#' @param ... used to handle superfluous arguments passed in using paramList.
#'
#' @return A random matrix to use in running \code{\link{RerF}}.
#'
#' @importFrom RcppZiggurat zrnorm
#'
#' @export
#'
#' @examples
#'
#' p <- 8
#' d <- 8
#' nmix <- 5
#' paramList <- list(p = p, d = d, nmix = nmix)
#' set.seed(8)
#' (a <- do.call(RandMatFRCN, paramList))
RandMatFRCN <- function(p, d, nmix, catMap = NULL, ...) {
  if (d > p) {
    stop("ERROR: parameter d is greater than the number of dimensions p.")
  }

  nnz <- nmix * d
  nz.rows <- integer(nnz)
  nz.cols <- integer(nnz)
  start.idx <- 1L
  for (i in seq.int(d)) {
    end.idx <- start.idx + nmix - 1L
    nz.rows[start.idx:end.idx] <- sample.int(p, nmix, replace = FALSE)
    nz.cols[start.idx:end.idx] <- i
    start.idx <- end.idx + 1L
  }

  if (is.null(catMap)) {
    randomMatrix <- cbind(nz.rows, nz.cols, zrnorm(nnz))
  } else {
    pnum <- catMap[[1L]][1L] - 1L
    isCat <- nz.rows > pnum
    for (j in (pnum + 1L):p) {
      isj <- nz.rows == j
      nz.rows[isj] <- sample(catMap[[j - pnum]], length(nz.rows[isj]),
        replace = TRUE
      )
    }
    randomMatrix <- cbind(nz.rows, nz.cols, zrnorm(nnz), deparse.level = 0)
  }
  return(randomMatrix)
}


#' Create a Random Matrix: ts-patch
#'
#'
#' @param p the number of dimensions.
#' @param d the number of desired columns in the projection matrix.
#' @param pwMin the minimum patch size to sample.
#' @param pwMax the maximum patch size to sample.
#' @param ... used to handle superfluous arguments passed in using paramList.
#'
#' @return A random matrix to use in running \code{\link{RerF}}.
#'
#' @export
#'
#' @examples
#'
#' p <- 8
#' d <- 8
#' pwMin <- 3
#' pwMax <- 6
#' paramList <- list(p = p, d = d, pwMin = pwMin, pwMax = pwMax)
#' set.seed(8)
#' (a <- do.call(RandMatTSpatch, paramList))
RandMatTSpatch <- function(p, d, pwMin, pwMax, ...) {
  if (pwMin > pwMax) {
    stop("ERROR: parameter pwMin is greater than pwMax.")
  }

  # pw holds all sizes of patch to filter on.  There will be d patches of
  # varying sizes
  pw <- sample.int(pwMax - pwMin, d, replace = TRUE) + pwMin

  # nnz is sum over how many points the projection will sum over
  nnz <- sum(pw)
  nz.rows <- integer(nnz) # vector to hold row coordinates of patch points
  nz.cols <- integer(nnz) # vector to hold column coordinates of patch points

  # Here we create the patches and store them
  start.idx <- 1L
  for (i in seq.int(d)) {
    pw.start <- sample.int(p, 1) # Sample where to start the patch
    end.idx <- start.idx + pw[i] - 1L # Set the ending point of the patch
    for (j in 1:pw[i]) {
      # Handle boundary cases where patch goes past end of ts
      if (j + pw.start - 1L > p) {
        end.idx <- j + start.idx - 1L
        break
      }
      nz.rows[j + start.idx - 1L] <- pw.start + j - 1L
      nz.cols[j + start.idx - 1L] <- i
    }
    start.idx <- end.idx + 1L
  }
  random.matrix <- cbind(nz.rows, nz.cols, rep(1L, nnz))
  random.matrix <- random.matrix[random.matrix[, 1] > 0, ] # Trim entries that are 0
}



#' Create a Random Matrix: image-patch
#'
#'
#' @param p the number of dimensions.
#' @param d the number of desired columns in the projection matrix.
#' @param ih the height (px) of the image.
#' @param iw the width (px) of the image.
#' @param pwMin the minimum patch size to sample.
#' @param pwMax the maximum patch size to sample.
#' @param ... used to handle superfluous arguments passed in using paramList.
#'
#' @return A random matrix to use in running \code{\link{RerF}}.
#'
#' @export
#'
#' @examples
#'
#' p <- 28^2
#' d <- 8
#' ih <- iw <- 28
#' pwMin <- 3
#' pwMax <- 6
#' paramList <- list(p = p, d = d, ih = ih, iw = iw, pwMin = pwMin, pwMax = pwMax)
#' set.seed(8)
#' (a <- do.call(RandMatImagePatch, paramList))
RandMatImagePatch <- function(p, d, ih, iw, pwMin, pwMax, ...) {
  if (pwMin > pwMax) {
    stop("ERROR: parameter pwMin is greater than pwMax.")
  }

  pw <- sample.int(pwMax - pwMin + 1L, 2 * d, replace = TRUE) + pwMin -
    1L
  sample.height <- ih - pw[1:d] + 1L
  sample.width <- iw - pw[(d + 1L):(2 * d)] + 1L
  nnz <- sum(pw[1:d] * pw[(d + 1L):(2 * d)])
  nz.rows <- integer(nnz)
  nz.cols <- integer(nnz)
  start.idx <- 1L
  for (i in seq.int(d)) {
    top.left <- sample.int(sample.height[i] * sample.width[i], 1L)
    top.left <- floor((top.left - 1L) / sample.height[i]) * (ih - sample.height[i]) +
      top.left
    # top.left <- floor((top.left - 1L)/sample.height[i]) + top.left
    end.idx <- start.idx + pw[i] * pw[i + d] - 1L
    nz.rows[start.idx:end.idx] <- sapply((1:pw[i + d]) - 1L, function(x) top.left:(top.left +
        pw[i] - 1L) + x * ih)
    nz.cols[start.idx:end.idx] <- i
    start.idx <- end.idx + 1L
  }
  # random.matrix <- cbind(nz.rows, nz.cols, sample(c(-1L,1L), nnz,
  # replace = TRUE))
  random.matrix <- cbind(nz.rows, nz.cols, rep(1L, nnz))
}


#' Create a Random Matrix: image-control
#'
#'
#' @param p the number of dimensions.
#' @param d the number of desired columns in the projection matrix.
#' @param ih the height (px) of the image.
#' @param iw the width (px) of the image.
#' @param pwMin the minimum patch size to sample.
#' @param pwMax the maximum patch size to sample.
#' @param ... used to handle superfluous arguments passed in using paramList.
#'
#' @return A random matrix to use in running \code{\link{RerF}}.
#'
#' @export
#'
#' @examples
#'
#' p <- 28^2
#' d <- 8
#' ih <- iw <- 28
#' pwMin <- 3
#' pwMax <- 6
#' paramList <- list(p = p, d = d, ih = ih, iw = iw, pwMin = pwMin, pwMax = pwMax)
#' set.seed(8)
#' (a <- do.call(RandMatImageControl, paramList))
RandMatImageControl <- function(p, d, ih, iw, pwMin, pwMax, ...) {
  if (pwMin > pwMax) {
    stop("ERROR: parameter pwMin is greater than pwMax.")
  }

  pw <- sample.int(pwMax - pwMin + 1L, 2 * d, replace = TRUE) + pwMin -
    1L
  nnzPerCol <- pw[1:d] * pw[(d + 1L):(2 * d)]
  sample.height <- ih - pw[1:d] + 1L
  sample.width <- iw - pw[(d + 1L):(2 * d)] + 1L
  nnz <- sum(nnzPerCol)
  nz.rows <- integer(nnz)
  nz.cols <- integer(nnz)
  start.idx <- 1L
  for (i in seq.int(d)) {
    end.idx <- start.idx + nnzPerCol[i] - 1L
    nz.rows[start.idx:end.idx] <- sample.int(p, nnzPerCol[i], replace = FALSE)
    nz.cols[start.idx:end.idx] <- i
    start.idx <- end.idx + 1L
  }
  # random.matrix <- cbind(nz.rows, nz.cols, sample(c(-1L,1L), nnz,
  # replace = TRUE))
  random.matrix <- cbind(nz.rows, nz.cols, rep(1L, nnz))
}



#' Create a Random Matrix: custom
#'
#'
#' @param p the number of dimensions.
#' @param d the number of desired columns in the projection matrix.
#' @param nnzSample a vector specifying the number of non-zeros to
#' sample at each \code{d}.  Each entry should be less than \code{p}.
#' @param nnzProb a vector specifying probabilities in one-to-one correspondance
#' with \code{nnzSample}.
#' @param ... used to handle superfluous arguments passed in using paramList.
#'
#' @return A random matrix to use in running \code{\link{RerF}}.
#'
#' @importFrom RcppZiggurat zrnorm
#'
#' @export
#'
#' @examples
#'
#' p <- 28
#' d <- 8
#' nnzSample <- 1:8
#' nnzProb <- 1 / 36 * 1:8
#' paramList <- list(p = p, d = d, nnzSample, nnzProb)
#' set.seed(8)
#' (a <- do.call(RandMatCustom, paramList))
RandMatCustom <- function(p, d, nnzSample, nnzProb, ...) {
  try({
    if (any(nnzSample > p) | any(nnzSample == 0)) {
      stop("nnzs per projection must be no more than the number of features.")
    }
  })
  nnzPerCol <- sample(nnzSample, d, replace = TRUE, prob = nnzProb)
  nnz <- sum(nnzPerCol)
  nz.rows <- integer(nnz)
  nz.cols <- integer(nnz)
  start.idx <- 1L
  for (i in seq.int(d)) {
    end.idx <- start.idx + nnzPerCol[i] - 1L
    nz.rows[start.idx:end.idx] <- sample.int(p, nnzPerCol[i], replace = FALSE)
    nz.cols[start.idx:end.idx] <- i
    start.idx <- end.idx + 1L
  }
  random.matrix <- cbind(nz.rows, nz.cols, zrnorm(nnz))
}


#' Default values passed to RandMat*
#'
#' Given the parameter list and the categorical map this function
#' populates the values of the parameter list accoding to our "best"
#' known general use case parameters.
#'
#' @param ncolX an integer denoting the number of columns in the design
#' matrix X.
#' @param paramList a list (possibly empty), to be populated with a set
#' of default values to be passed to a RandMat* function.
#' @param cat.map a list specifying which columns in X correspond to the
#' same one-of-K encoded feature. Each element of cat.map is a numeric
#' vector specifying the K column indices of X corresponding to the same
#' categorical feature after one-of-K encoding. All one-of-K encoded
#' features in X must come after the numeric features. The K encoded
#' columns corresponding to the same categorical feature must be placed
#' contiguously within X. The reason for specifying cat.map is to adjust
#' for the fact that one-of-K encoding cateogorical features results in
#' a dilution of numeric features, since a single categorical feature is
#' expanded to K binary features. If cat.map = NULL, then RerF assumes
#' all features are numeric (i.e. none of the features have been
#' one-of-K encoded).
#'
#' @return If \code{cat.map} is NULL, then
#' \itemize{
#' \item \code{p} is set to the number of columns of \code{X}
#' \item \code{d} is set to the ceiling of the square root of the number of columns of \code{X}
#' \item \code{sparsity}: if \eqn{\code{ncol(X)} \ge 10}, then sparsity is set
#' to 3 / \code{ncol{X}}, otherwise it is set to 1 / \code{ncol(X)}.
#' \item \code{prob} defaults to 0.5.
#' }
#'
#' @keywords internal
#'

defaults <- function(ncolX, paramList, cat.map) {
  if (is.null(paramList$p) || is.na(paramList$p)) {
    paramList$p <- ifelse(is.null(cat.map),
      ncolX,
      length(cat.map) + cat.map[[1L]][1L] - 1L
    )
  }

  if (is.null(paramList$d) || is.na(paramList$d)) {
    paramList$d <- ifelse(is.null(cat.map),
      ceiling(sqrt(ncolX)),
      ceiling(sqrt(length(cat.map) + cat.map[[1L]][1L] - 1L))
    )
  }

  if (is.null(paramList$sparsity) || is.na(paramList$sparsity)) {
    paramList$sparsity <- ifelse(is.null(cat.map),
      ifelse(ncolX >= 10, 3 / ncolX, 1 / ncolX),
      ifelse(length(cat.map) + cat.map[[1L]][1L] - 1L >= 10,
        3 / (length(cat.map) + cat.map[[1L]][1L] - 1L),
        1 / (length(cat.map) + cat.map[[1L]][1L] - 1L)
      )
    )
  }

  if (is.null(paramList$prob) || is.na(paramList$prob)) {
    paramList$prob <- 0.5
  }
  return(paramList)
}


#' Create rotation matrix used to determine mtry features.
#'
#' This function is the default option to make the projection matrix for
#' unsupervised random forest. The sparseM matrix is the projection
#' matrix.  The creation of this matrix can be changed, but the nrow of
#' sparseM should remain p.  The ncol of the sparseM matrix is currently
#' set to mtry but this can actually be any integer > 1; can even be
#' greater than p.
#'
#'
#' @param p the number of dimensions.
#' @param d the number of desired columns in the projection matrix.
#' @param sparsity a real number in \eqn{(0,1)} that specifies the distribution of non-zero elements in the random matrix.
#' @param ... used to handle superfluous arguments passed in using paramList.
#'
#' @return rotationMatrix the matrix used to determine which mtry features or combination of features will be used to split a node.
#'
#'


makeAB <- function(p, d, sparsity, ...) {
  nnzs <- round(p * d * sparsity)
  sparseM <- matrix(0L, nrow = p, ncol = d)
  featuresToTry <- sample(1:p, d, replace = FALSE)
  # the commented line below creates linear combinations of features to try
  # sparseM[sample(1L:(p*d),nnzs, replace=FALSE)]<-sample(c(1L,-1L),nnzs,replace=TRUE)
  # the for loop below creates a standard random forest set of features to try
  for (j in 1:d) {
    sparseM[featuresToTry[j], j] <- 1
  }
  # The below returns a matrix after removing zero columns in sparseM.
  ind <- which(sparseM != 0, arr.ind = TRUE)
  return(cbind(ind, sparseM[ind]))
}



#' The 2-d Gabor weight function
#'
#' 
#' @param x x coordinate 
#' @param y y coordinate 
#' @param alpha scaling factor of the Gaussian envelope
#' @param betax \eqn{\beta_x} decay factor in the x direction
#' @param betay \eqn{\beta_y} decay factor in the y direction
#' @param f frequency of the cosine factor
#' @param phi phase offset in the cosine factor
#' @param x0 mean in the x direction
#' @param y0 mean in the y direction
#' @param tau the angle of rotation for x and y
#'
#' @return a weight in the form of a scaler.
#'
#' @export
#' 
#' @references @unpublished{Goodfellow-et-al-2016-Book,
#'   title={Deep Learning},
#'   author={Ian Goodfellow, Yoshua Bengio, and Aaron Courville},
#'   note={Book in preparation for MIT Press},
#'   url={http://www.deeplearningbook.org},
#'   year={2016}
#' }
#'
#' @examples
#'
#' p <- 28^2
#' d <- 8
#' ih <- iw <- 28
#' pwMin <- 3
#' pwMax <- 6
#' paramList <- list(p = p, d = d, ih = ih, iw = iw, pwMin = pwMin, pwMax = pwMax)
#' set.seed(8)
#' (a <- do.call(RandMatImagePatch, paramList))

Gw <- function(x, y, alpha, betax, betay, f, phi, x0, y0, tau, betaxy = 0) {

  xp <-  (x - x0) * cos(tau) + (y - y0) * sin(tau)
  yp <- -(x - x0) * sin(tau) + (y - y0) * cos(tau)

  alpha * exp(- betax * xp^2 - betay * yp^2 + (betaxy * xp * yp)) * 
    cos(f * xp + phi)
}


#' Sample the hyper-parameters for the Gabor weight function
#'
#' 
#' @param n the number of samples
#' @param galpha the shape parameter for alpha
#' @param gbeta the shape parmeter for alpha
#' @param igalpha the shape parameter for betax and betay
#' @param igbeta the rate parameter for betax and betay
#'
#' @return a list of parameters for the Gabor Weight function.
#'
#' @export
#' 
#' @references @unpublished{Goodfellow-et-al-2016-Book,
#'   title={Deep Learning},
#'   author={Ian Goodfellow, Yoshua Bengio, and Aaron Courville},
#'   note={Book in preparation for MIT Press},
#'   url={http://www.deeplearningbook.org},
#'   year={2016}
#' }
#'
#' @examples
#'
#' params <- gabSamp()
#' ##  must specify grid
#' x <- seq(-4,4, length = 9)
#' xy <- expand.grid(x,x)
#' params$x <- xy[, 1]
#' params$y <- xy[, 2]
#' gw <- do.call(Gw, params)
#' 
#' 

gabSamp <- function(n = 1, galpha = 3, gbeta = 1, igalpha = 1, igbeta = 0.5, iw) { 
  out <- list()

  out$alpha <- 1 # rgamma(n, shape = galpha, rate = gbeta)
  out$betax <- rinvgamma(n, shape = igalpha, scale = igbeta)
  out$betay <- rinvgamma(n, shape = igalpha, scale = igbeta)
  out$f <-  runif(n, min = floor(iw / 2), max = 8 * iw)
  out$phi <- runif(n, min = 0, max = 2 * pi)
  out$x0 <- runif(n, min = -2.5, max = 2.5)#rnorm(n, mean = 0, sd = 0.75)
  out$y0 <- runif(n, min = -2.5, max = 2.5)#rnorm(n, mean = 0, sd = 0.75)
  out$tau <- runif(n, min = 0, max = 2 * pi)


  #if(w == 1){
  #  # Letter B
  #  S <- riwish(v = 2, S = diag(2))
  #  sigx2 <- S[1]
  #  sigy2 <- S[4]
  #  rho <- S[2] / (sqrt(sigx2) * sqrt(sigy2))
  #  out$betax <- 1 / (2 * sigx2  * (1 - rho^2))
  #  out$betay <- 1 / (2 * sigy2  * (1 - rho^2))
  #  out$betaxy <- rho / (sqrt(sigx2) * sqrt(sigy2) * (1 - rho^2))
  #}

  #if(w == 2){
  #  # Letter C
  #  S <- riwish(v = 2, S = diag(2))
  #  sigx2 <- S[1]
  #  sigy2 <- S[4]
  #  rho <- S[2] / (sqrt(sigx2) * sqrt(sigy2))
  #  out$betax <- 1 / (2 * sigx2  * (1 - rho^2))
  #  out$betay <- 1 / (2 * sigy2  * (1 - rho^2))
  #  out$betaxy <- 0
  #}

  #if(w == 3){
  #  # letter D
  #  S <- riwish(v = 2, S = diag(2))
  #  sigx2 <- S[1]
  #  sigy2 <- S[4]
  #  out$betax <- 1 / (2 * sigx2)
  #  out$betay <- 1 / (2 * sigy2)
  #  out$betaxy <- 0
  #}

  return(out)
}



#' Create a Random Matrix: For an Image with Gabor weighting
#' 
#' The pathces are specified to be square, because the Gabor filter
#' takes care of the "squishing".
#' Also the minimum patch size should be 3x3.
#'
#' @param p the number of dimensions.
#' @param d the number of desired columns in the projection matrix.
#' @param ih the height (px) of the image.
#' @param iw the width (px) of the image.
#' @param pwMin the minimum patch size to sample.
#' @param pwMax the maximum patch size to sample.
#' @param ... used to handle superfluous arguments passed in using paramList.
#'
#' @return A random matrix to use in running \code{\link{RerF}}.
#'
#' @export
#'
#' @examples
#'
#' ih <- iw <- 28
#' p <- 28^2
#' d <- 28
#' paramList <- list(p = p, d = d, ih = ih, iw = iw)
#' set.seed(8)
#' (a <- do.call(RandMatImageGabor, paramList))
#' for(i in 1:d){
#'   plot(raster::raster(matrix(a[a[, 2] == i,][, 3L] , 28, 28)))
#'   Sys.sleep(0.6)
#' }
#' 

RandMatGabor <- function(p, d, ih, iw, tol = .Machine$double.eps, ...) {

  ## Sample Gabor Filter
  gx <-  seq(-2.5, 2.5, length = iw) 
  gy <-  seq(2.5, -2.5, length = ih)
  gridXY <- expand.grid(gx, gy)
  gaborBank <- list()
  for(i in 1:d){
    params <- gabSamp(w = whG) ## 3 = letter c
    params$x <- gridXY[, 1]
    params$y <- gridXY[, 2]
    gaborBank[[i]] <- do.call(Gw, params)
    #if(DEBUG){
    #  plot(raster::raster(matrix(gaborBank[[i]], 28, 28)))
    #  Sys.sleep(1)
    #}
  }

#plot(raster::raster(matrix(X, 28, 28))) 
#plot(raster::raster(matrix(G1, 28, 28))) 
#plot(raster::raster(matrix(X * G1, 28, 28))) 

  nz.rows <- rep(1:p, d)
  nz.cols <- rep(1:d, each = p)

  w <- Reduce('c', gaborBank)
  w[w <= tol & w >= -tol] <- 0 ## NOTE: This is not efficient

  ## remove w within delta ball of zero to save space
  random.matrix <- 
    as.matrix(cbind(nz.rows = nz.rows, nz.cols = nz.cols, w = w))[w != 0, ]
}
