#' Tabulate the unique feature combinations used in a single RerF tree
#'
#' Computes feature importance of every unique feature used to make a split in a single tree.
#'
#' @param tree a single tree from a trained RerF model with argument store.impurity = TRUE.
#' @param unique.projections a list of all of the unique split projections used in the RerF model.
#'
#' @return feature.counts
#'
#' @examples
#' @examples
#' library(rerf)
#' X <- iris[, -5]
#' Y <- iris[[5]]
#' store.impurity <- TRUE
#' FUN <- RandMatContinuous
#' forest <- RerF(X, Y, FUN = FUN, num.cores = 1L, store.impurity = store.impurity)
#' FeatureImportance(forest, num.cores = 1L, featureCounts = TRUE)

RunFeatureImportanceCounts <- function(tree, unique.projections) {
  num.proj <- length(unique.projections)
  feature.counts <- double(num.proj)

  for (nd in tree$treeMap[tree$treeMap > 0L]) {
    index.low <- tree$matAindex[nd] + 1L
    index.high <- tree$matAindex[nd + 1L]
    projection.idx <-
      which(unique.projections %in%
        lapply(list(tree$matAstore[index.low:index.high]), getFeatures))
    feature.counts[projection.idx] <-
      feature.counts[projection.idx] + 1
  }
  return(feature.counts)
}
