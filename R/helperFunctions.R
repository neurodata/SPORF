#' Extract feature indicies from the sparse projection vector.
#'
#' A helper function to extract the feature indices from the projection
#' vector stored in a tree object.
#'
#' @param x a list of unique.projections from the intermediate steps of
#' the FeatureImportance function.
#'
#' @return list of unique feature combinations
#'

getFeatures <- function(x) {
  s <- seq(1, length(x), by = 2)
  return(x[s])
}

#' Extract feature weights from the sparse projection vector.
#'
#' A helper function to extract the feature weights from the projection
#' vector stored in a tree object.
#'
#' @param x a list of unique.projections from the intermediate steps of
#' the FeatureImportance function.
#'
#' @return list of unique feature weights
#'

getWeights <- function(x) {
  s <- seq(2, length(x), by = 2)
  return(x[s])
}

#' Change the sign of the weights
#'
#' A helper function to extract the feature weights from the projection
#' vector stored in a tree object. Used in
#' \code{RunFeatureImportanceBinary}.
#'
#' @param x a list of unique.projections from the intermediate steps of
#' the FeatureImportance function.
#'
#' @return x with sign of weights flipped.
#'


flipWeights <- function(x) {
  s <- seq(2, length(x), by = 2)
  x[s] <- -x[s]
  return(x)
}


#' Remove unique projections that are equivalent due to a rotation of 180
#' degrees.
#'
#' This function finds the projections that are equivalent via a 180
#' degree rotation and removes the duplicates.
#'
#' @param p the number of features in the original data.  This can be
#' obtained from a forest object via \code{forest$params$paramList$p}.
#' @param unique.projections a list of projections from intermediate
#' steps of the \code{\link{FeatureImportance}} function.
#'
#' @return unique.projections a list which is a subset of the input.
#'
#' @seealso \code{\link{FeatureImportance}}
#'
#'

uniqueByEquivalenceClass <- function(p, unique.projections) {

  ## the matrix of weights (w)
  w <- matrix(0,
    ncol = p,
    nrow = length(unique.projections)
  )

  for (i in 1:length(unique.projections)) {
    for (j in seq(1, length(unique.projections[[i]]), by = 2)) {
      w[i, unique.projections[[i]][j]] <- unique.projections[[i]][j + 1]
    }
  }

  out <- vector("list", 1 / 2 * (length(unique.projections) - 1) *
    length(unique.projections))
  k <- 1
  for (i in 1:nrow(w)) {
    for (j in i:nrow(w)) {
      if (all(w[i, ] == -w[j, ])) {
        out[[k]] <- c(i, j)
        k <- k + 1
      }
    }
  }

  ind <- !sapply(out, is.null)
  out <- Reduce(rbind, out)

  unique.projections[out[, 2]] <- NULL

  return(unique.projections)
}
