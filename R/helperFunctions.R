#' Extract feature indicies from the sparse projection vector.
#'
#' A helper function to extract the feature indices from the projection
#' vector stored in a tree object.
#'
#' @param a list of unique.projections from the intermediate steps of
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
#' @param a list of unique.projections from the intermediate steps of
#' the FeatureImportance function.
#'
#' @return list of unique feature weights
#'

getWeights <- function(x) {
  s <- seq(2, length(x), by = 2)
  return(x[s])
}

#' Remove Unique Projections that are equivalent due to a rotation of 180
#' degrees.
#'
#' This function finds the projections that are equivalent via a 180
#' degree rotation and removes the duplicates.
#'
#' @param unique.projection a list of projections from intermediate
#' steps of the \code{\link{FeatureImportance}} function.
#'
#' @return a list which is a subset of the input.
#'
#' @seealso \code{\link{FeatureImportance}}
#'
#' @import Matrix

uniqueByEquivalenceClass <- function(unique.projections) {

  ## the matrix of weights (w)
  w <- as(matrix(0,
    ncol = forest$params$paramList$p,
    nrow = length(unique.projections)
  ), "sparseMatrix")

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
