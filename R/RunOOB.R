#' Predict class labels on out-of-bag observations using a single tree.
#'
#' This is the base function called by OOBPredict.
#'
#' @param X an n sample by d feature matrix (preferable) or data frame which was used to train the provided forest.
#' @param tree a tree from a forest returned by RerF.
#'
#' @return out prediction matrix used by OOBPredict
#'

RunOOB <- function(X, tree, task = 'classification') {
  n.all <- nrow(X)

  if (task == 'classification')
    num.classes <- ncol(tree$ClassProb)

  # Get OOB samples
  X <- X[tree$ind, , drop = FALSE]

  # Predict OOB samples
  predictions <- RunPredict(X, tree, task=task)

  # Create a matrix for all of samples
  if (task == 'classification') {
    out <- matrix(0, nrow = n.all, ncol = num.classes)
  } else {
    out <- matrix(0, nrow = n.all, ncol = 1)
  }
  out[tree$ind, ] <- predictions
  return(out)
}
