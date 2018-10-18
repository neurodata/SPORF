#' Create a Random Matrix: Binary
#'
#'
#' @param p the number of dimensions.
#' @param d the number of desired columns in the projection matrix.
#' @param sparsity a real number in \eqn{(0,1)} that specifies the distribution of non-zero elements in the random matrix.
#' @param prob a probability \eqn{\in (0,1)} used for sampling from
#' \eqn{{-1,1}} where \code{prob = 0} will only sample -1 and \code{prob = 1} will only sample 1.
#' @param catMap a list specifying specifies which one-of-K encoded columns in X correspond to the same categorical feature. 
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
#' (a <- RandMatBinary(p, p + 1, sparsity, prob))
#'

RandMatBinary <- function(p, d, sparsity, prob, catMap = NULL) {
  if(d > p){
    stop("ERROR: parameter d is greater than the number of dimensions p.")
  }

  nnzs <- round(p * d * sparsity)
  ind <- sort(sample.int((p * d), nnzs, replace = FALSE))
  
  ## Determine if categorical variables need to be taken into
  ## consideration
  if (is.null(catMap)) {
    randomMatrix <- cbind(((ind - 1L)%%p) + 1L, floor((ind - 1L)/p) + 
      1L, sample(c(1L, -1L), nnzs, replace = TRUE, prob = c(prob, 
      1 - prob)))
  } else {
    pnum <- catMap[[1L]][1L] - 1L
    rw <- ((ind - 1L)%%p) + 1L
    isCat <- rw > pnum
    for (j in (pnum + 1L):p) {
      isj <- rw == j
      rw[isj] <- sample(catMap[[j - pnum]], length(rw[isj]), replace = TRUE)
    }
    randomMatrix <- cbind(rw, floor((ind - 1L)/p) + 1L, sample(c(1L, 
      -1L), nnzs, replace = TRUE, prob = c(prob, 1 - prob)), deparse.level = 0)
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
#' (a <- RandMatContinuous(p, d, rho))
#'

RandMatContinuous <- function(p, d, sparsity, catMap = NULL) {
  if(d > p){
    stop("ERROR: parameter d is greater than the number of dimensions p.")
    }

  nnzs <- round(p * d * sparsity)
  ind <- sort(sample.int((p * d), nnzs, replace = FALSE))
  
  if (is.null(catMap)) {
    randomMatrix <- cbind(((ind - 1L)%%p) + 1L, floor((ind - 1L)/p) + 
      1L, zrnorm(nnzs))
  } else {
    pnum <- catMap[[1L]][1L] - 1L
    rw <- ((ind - 1L)%%p) + 1L
    isCat <- rw > pnum
    for (j in (pnum + 1L):p) {
      isj <- rw == j
      rw[isj] <- sample(catMap[[j - pnum]], length(rw[isj]), replace = TRUE)
    }
    randomMatrix <- cbind(rw, floor((ind - 1L)/p) + 1L, zrnorm(nnzs), 
      deparse.level = 0)
  }
  return(randomMatrix)
}


#' Create a Random Matrix: Random Forest (RF)
#'
#'
#' @param p the number of dimensions.
#' @param d the number of desired columns in the projection matrix.
#' @param catMap a list specifying specifies which one-of-K encoded columns in X correspond to the same categorical feature. 
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
#'

RandMatRF <- function(p, d, catMap = NULL) {
  if(d > p){
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
#'

RandMatPoisson <- function(p, d, lambda, catMap = NULL) {
  if(d > p){
    stop("ERROR: parameter d is greater than the number of dimensions p.")
    }

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
      replace = TRUE))
  } else {
    pnum <- catMap[[1L]][1L] - 1L
    isCat <- nz.rows > pnum
    for (j in (pnum + 1L):p) {
      isj <- nz.rows == j
      nz.rows[isj] <- sample(catMap[[j - pnum]], length(nz.rows[isj]), 
        replace = TRUE)
    }
    randomMatrix <- cbind(nz.rows, nz.cols, sample(c(-1L, 1L), nnz, 
      replace = TRUE), deparse.level = 0)
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
#'

RandMatFRC <- function(p, d, nmix, catMap = NULL) {
  if(d > p){
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
    randomMatrix <- cbind(nz.rows, nz.cols, runif(nnz, -1, 1))
  } else {
    pnum <- catMap[[1L]][1L] - 1L
    isCat <- nz.rows > pnum
    for (j in (pnum + 1L):p) {
      isj <- nz.rows == j
      nz.rows[isj] <- sample(catMap[[j - pnum]], length(nz.rows[isj]), 
        replace = TRUE)
    }
    randomMatrix <- cbind(nz.rows, nz.cols, runif(nnz, -1, 1), 
      deparse.level = 0)
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
#'

RandMatFRCN <- function(p, d, nmix, catMap = NULL) {
  if(d > p){
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
        replace = TRUE)
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
#'

RandMatTSpatch <- function(p, d, pwMin, pwMax) {
  if(d > p){
    stop("ERROR: parameter d is greater than the number of dimensions p.")
  }

  if(pwMin > pwMax){
    stop("ERROR: parameter pwMin is greater than pwMax.")
  }

  # pw holds all sizes of patch to filter on.  There will be d patches of
  # varying sizes
  pw <- sample.int(pwMax - pwMin, d, replace = TRUE) + pwMin
  
  # nnz is sum over how many points the projection will sum over
  nnz <- sum(pw)
  nz.rows <- integer(nnz)  # vector to hold row coordinates of patch points
  nz.cols <- integer(nnz)  # vector to hold column coordinates of patch points
  
  # Here we create the patches and store them
  start.idx <- 1L
  for (i in seq.int(d)) {
    pw.start <- sample.int(p, 1)  # Sample where to start the patch
    end.idx <- start.idx + pw[i] - 1L  # Set the ending point of the patch
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
  random.matrix <- random.matrix[random.matrix[, 1] > 0, ]  # Trim entries that are 0
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
#'

RandMatImagePatch <- function(p, d, ih, iw, pwMin, pwMax) {
  if(d > p){
    stop("ERROR: parameter d is greater than the number of dimensions p.")
  }

  if(pwMin > pwMax){
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
    top.left <- floor((top.left - 1L)/sample.height[i]) * (ih - sample.height[i]) + 
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
#'

RandMatImageControl <- function(p, d, ih, iw, pwMin, pwMax) {
  if(d > p){
    stop("ERROR: parameter d is greater than the number of dimensions p.")
  }

  if(pwMin > pwMax){
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
#' nnzProb <- 1/36 * 1:8
#' paramList <- list(p = p, d = d, nnzSample, nnzProb)
#' set.seed(8)
#' (a <- do.call(RandMatCustom, paramList))
#'

RandMatCustom <- function(p, d, nnzSample, nnzProb) {
  if(d > p){
    stop("ERROR: parameter d is greater than the number of dimensions p.")
  }

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


