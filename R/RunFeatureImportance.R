#' Compute Feature Importance of a single RerF tree
#'
#' Computes feature importance of every unique feature used to make a split in a single tree.
#'
#' @param tree a single tree from a trained RerF model with argument store.impurity = TRUE.
#' @param unique.projections a list of all of the unique split projections used in the RerF model.
#'
#' @return feature.imp

RunFeatureImportance <- function(tree, unique.projections) {
  num.proj <- length(unique.projections)

  feature.imp <- double(num.proj)
  for (nd in tree$treeMap[tree$treeMap > 0L]) {
    index.low <- tree$matAindex[nd] + 1L
    index.high <- tree$matAindex[nd + 1L]
    projection.idx <- which(unique.projections %in% list(tree$matAstore[index.low:index.high]))
    feature.imp[projection.idx] <- feature.imp[projection.idx] + tree$delta.impurity[nd]
  }
  return(feature.imp)
}
