#' Predict class labels on a test set using a single tree.
#'
#' This is the base function called by Predict.
#'
#' @param X an n sample by d feature matrix (preferable) or data frame which was used to train the provided forest.
#' @param tree a tree from a forest returned by RerF.
#'
#' @return predictions an n length vector of prediction based on the tree provided to this function
#'
#' @examples
#' \dontrun{
#' require(rerf)
#' X <- as.matrix(iris[, -5])
#' Y <- as.numeric(iris[, 5])
#' paramList <- list(p = 4, d = 2, sparsity = 0.25, prob = 0.5)
#'
#' forest <- list(trees = NULL, labels = NULL, params = NULL)
#' (forest$labels <- sort(unique(Y)))
#'
#' (num.class <- length(forest$labels))
#' (classCt <- cumsum(tabulate(Y, num.class)))
#' Cindex <- vector("list", num.class)
#' paramBT <- list(X = X, Y = Y, FUN = rerf::RandMatBinary, paramList =
#' paramList, min.parent = 1L, max.depth = 6L, bagging = 0.2, class.ct =
#' classCt, class.ind = NULL, replacement = TRUE, stratify = FALSE,
#' store.oob = FALSE, store.impurity = FALSE, progress = FALSE, rotate =
#' FALSE, scaleAtNode = TRUE)
#' list2env(paramBT, .GlobalEnv)
#'
#' set.seed(10)
#' tree <- forest$trees[[1]] <- do.call(rerf:::BuildTree, paramBT)
#' rerf:::RunPredict(X, forest$trees[[1]])
#' }
RunPredict <- function(X, tree) {
  n.all <- nrow(X)

  tm <- 0L
  currentNode <- 0L
  curr_ind <- 0L
  num.classes <- ncol(tree$ClassProb)
  n <- nrow(X)

  # do we need to rotate the data?
  if (!is.null(tree$rotmat)) {
    if (is.null(tree$rotdims)) {
      X[] <- X %*% tree$rotmat
    } else {
      X[, tree$rotdims] <- X[, tree$rotdims] %*% tree$rotmat
    }
  }

  predictions <- matrix(0, nrow = n, ncol = num.classes)

  Xnode <- double(n)
  numNodes <- length(tree$treeMap)
  Assigned2Node <- vector("list", numNodes)
  Assigned2Node[[1L]] <- 1L:n
  for (m in 1:numNodes) {
    nodeSize <- length(Assigned2Node[[m]])
    if (nodeSize > 0L) {
      if ((tm <- tree$treeMap[m]) > 0L) { ## if node is not terminal, then split data
        indexHigh <- tree$matAindex[tm + 1L]
        indexLow <- tree$matAindex[tm] + 1L
        s <- (indexHigh - indexLow + 1L) / 2L

        if (!is.null(tree$scalingFactors[tm])) { ## for when the node data has been scaled to [0,1].
          Xnode[1:nodeSize] <-
            Scale01(X[Assigned2Node[[m]], tree$matAstore[indexLow:indexHigh][(1:s) * 2L - 1L], drop = FALSE],
                  scalingFactors = tree$scalingFactors[[tm]]) %*%
            tree$matAstore[indexLow:indexHigh][(1:s) * 2L]
        } else {
          Xnode[1:nodeSize] <-
            X[Assigned2Node[[m]], tree$matAstore[indexLow:indexHigh][(1:s) * 2L - 1L], drop = FALSE] %*%
            tree$matAstore[indexLow:indexHigh][(1:s) * 2L]
        }

        moveLeft <- Xnode[1L:nodeSize] <= tree$CutPoint[tm]
        Assigned2Node[[tm * 2L]] <- Assigned2Node[[m]][moveLeft]
        Assigned2Node[[tm * 2L + 1L]] <- Assigned2Node[[m]][!moveLeft]
      } else { ## else run prediction
        predictions[Assigned2Node[[m]], ] <- rep(tree$ClassProb[tm * -1L, ], each = length(Assigned2Node[[m]]))
      }
    }
    Assigned2Node[m] <- list(NULL)
  }
  return(predictions)
}
