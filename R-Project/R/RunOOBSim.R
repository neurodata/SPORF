#' Predict similarity on out-of-bag observations using a single tree.
#'
#' This is the base function called by OOBPredict.
#'
#' @param X an n sample by d feature matrix (preferable) or data frame which was used to train the provided forest.
#' @param tree a tree from a forest returned by RerF.
#'
#' @return predictions prediction matrix used by OOBPredict
#'

RunOOBSim <-
  function(X, tree) {
    n <- nrow(X)
    num.oob <- length(tree$ind)
    is.oob <- rep(FALSE, n)
    is.oob[tree$ind] <- TRUE

    # don't remove inbag points because we also want similarities between inbag and oob samples
    # X <- X[tree$ind, , drop = F]

    # do we need to rotate the data?
    if (!is.null(tree$rotmat)) {
      if (is.null(tree$rotdims)) {
        X[] <- X %*% tree$rotmat
      } else {
        X[, tree$rotdims] <- X[, tree$rotdims] %*% tree$rotmat
      }
    }

    predictions <- matrix(as.double(NA), nrow = n, ncol = n)
    diag(predictions) <- 1

    currentNode <- 0L
    curr_ind <- 0L
    tm <- 0L
    leafNodeIdx <- integer(n)

    Xnode <- double(n)
    numNodes <- length(tree$treeMap)
    Assigned2Node <- vector("list", numNodes)
    Assigned2Node[[1L]] <- 1L:n
    for (m in 1:numNodes) {
      nodeSize <- length(Assigned2Node[[m]])
      if (nodeSize > 0L) {
        if ((tm <- tree$treeMap[m]) > 0L) {
          indexHigh <- tree$matAindex[tm + 1L]
          indexLow <- tree$matAindex[tm] + 1L
          s <- (indexHigh - indexLow + 1L) / 2L
          Xnode[1:nodeSize] <- X[Assigned2Node[[m]], tree$matAstore[indexLow:indexHigh][(1:s) * 2L - 1L], drop = F] %*%
            tree$matAstore[indexLow:indexHigh][(1:s) * 2L]
          moveLeft <- Xnode[1L:nodeSize] <= tree$CutPoint[tm]
          Assigned2Node[[tm * 2L]] <- Assigned2Node[[m]][moveLeft]
          Assigned2Node[[tm * 2L + 1L]] <- Assigned2Node[[m]][!moveLeft]
        } else {
          leafNodeIdx[Assigned2Node[[m]]] <- tm * -1L
        }
      }
      Assigned2Node[m] <- list(NULL)
    }

    # compute the lower triangular portion of the symmetric similarity matrix
    for (j in 1L:(n - 1L)) {
      # if j is an oob sample, then compute similarity to all other points in traiing set
      if (is.oob[j]) {
        for (i in j:n) {
          if (leafNodeIdx[i] >= leafNodeIdx[j]) {
            predictions[i, j] <- tree$leafSimilarity[leafNodeIdx[i], leafNodeIdx[j]]
          } else {
            predictions[i, j] <- tree$leafSimilarity[leafNodeIdx[j], leafNodeIdx[i]]
          }
        }
      } else {
        # only compute similarity to oob samples
        oob.greater <- tree$ind[tree$ind > j]
        for (i in oob.greater) {
          if (leafNodeIdx[i] >= leafNodeIdx[j]) {
            predictions[i, j] <- tree$leafSimilarity[leafNodeIdx[i], leafNodeIdx[j]]
          } else {
            predictions[i, j] <- tree$leafSimilarity[leafNodeIdx[j], leafNodeIdx[i]]
          }
        }
      }
    }

    # old code for only similarities *within* oob samples (i.e. excludes similarities to in bag samples)
    # if (num.oob > 1L) {
    #   currentNode <- 0L
    #   curr_ind <- 0L
    #   tm <- 0L
    #   leafNodeIdx <- integer(num.oob)
    #
    #   Xnode <- double(num.oob)
    #   numNodes <- length(tree$treeMap)
    #   Assigned2Node <- vector("list", numNodes)
    #   Assigned2Node[[1L]] <- 1L:num.oob
    #   for (m in 1:numNodes) {
    #     nodeSize <- length(Assigned2Node[[m]])
    #     if (nodeSize > 0L) {
    #       if ((tm <- tree$treeMap[m]) > 0L) {
    #         indexHigh <- tree$matAindex[tm + 1L]
    #         indexLow <- tree$matAindex[tm] + 1L
    #         s <- (indexHigh - indexLow + 1L) / 2L
    #         Xnode[1:nodeSize] <- X[Assigned2Node[[m]], tree$matAstore[indexLow:indexHigh][(1:s) * 2L - 1L], drop = F] %*%
    #           tree$matAstore[indexLow:indexHigh][(1:s) * 2L]
    #         moveLeft <- Xnode[1L:nodeSize] <= tree$CutPoint[tm]
    #         Assigned2Node[[tm * 2L]] <- Assigned2Node[[m]][moveLeft]
    #         Assigned2Node[[tm * 2L + 1L]] <- Assigned2Node[[m]][!moveLeft]
    #       } else {
    #         leafNodeIdx[Assigned2Node[[m]]] <- tm * -1L
    #       }
    #     }
    #     Assigned2Node[m] <- list(NULL)
    #   }
    #
    #   for (j in 1L:(num.oob - 1L)) {
    #     for (i in (j + 1L):num.oob) {
    #       if (leafNodeIdx[i] >= leafNodeIdx[j]) {
    #         predictions[tree$ind[i], tree$ind[j]] <- tree$leafSimilarity[leafNodeIdx[i], leafNodeIdx[j]]
    #       } else {
    #         predictions[tree$ind[i], tree$ind[j]] <- tree$leafSimilarity[leafNodeIdx[j], leafNodeIdx[i]]
    #       }
    #     }
    #   }
    # }

    predictions[upper.tri(predictions)] <- t(predictions)[upper.tri(predictions)]

    return(predictions)
  }
