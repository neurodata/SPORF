#' Predict class labels on a test set using a single tree.
#'
#' This is the base function called by Predict.
#'
#' @param X an n sample by d feature matrix (preferable) or data frame which was used to train the provided forest.
#' @param Xtrain  an n by d numeric matrix (preferable) or data frame. This should be the same data matrix/frame used to train the forest. If provided, then the output will be an ntest-by-ntrain matrix of similarities between training and test points (Xtrain=NULL)
#' @param tree a tree from a forest returned by RerF.
#' @param OOB  if TRUE and Xtrain is provided, then the tree predicts only the similarities of the out-of-bag training points to the test points. (OOB=TRUE)
#'
#' @return predictions an n length vector of prediction based on the tree provided to this function
#'

RunPredictSim <-
  function(X, Xtrain, tree, OOB) {
    tm <- 0L
    currentNode <- 0L
    curr_ind <- 0L
    if (!is.null(Xtrain)) {
      ntrain <- nrow(Xtrain)
      if (OOB) {
        # predict.idx <- which(!(1L:ntrain %in% tree$ind))
        # num.oob <-length(predict.idx)
        # Xtrain <- Xtrain[predict.idx, , drop = F]
        num.oob <- length(tree$ind)
        Xtrain <- Xtrain[tree$ind, , drop = F]
      }
      ntest <- nrow(X)
      X <- rbind(Xtrain, X)
    }
    n <- nrow(X)

    # do we need to rotate the data?
    if (!is.null(tree$rotmat)) {
      if (is.null(tree$rotdims)) {
        X[] <- X %*% tree$rotmat
      } else {
        X[, tree$rotdims] <- X[, tree$rotdims] %*% tree$rotmat
      }
    }

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

    if (is.null(Xtrain)) {
      predictions <- matrix(as.double(NA), nrow = n, ncol = n)
      diag(predictions) <- 1
      for (j in 1L:(n - 1L)) {
        for (i in (j + 1L):n) {
          if (leafNodeIdx[i] >= leafNodeIdx[j]) {
            predictions[i, j] <- tree$leafSimilarity[leafNodeIdx[i], leafNodeIdx[j]]
          } else {
            predictions[i, j] <- tree$leafSimilarity[leafNodeIdx[j], leafNodeIdx[i]]
          }
        }
      }
    } else {
      predictions <- matrix(as.double(NA), nrow = ntest, ncol = ntrain)
      if (OOB) {
        for (j in 1L:num.oob) {
          oob.idx <- tree$ind[j]
          # oob.idx <- predict.idx[j]
          for (i in 1L:ntest) {
            if (leafNodeIdx[num.oob + i] >= leafNodeIdx[j]) {
              predictions[i, oob.idx] <- tree$leafSimilarity[leafNodeIdx[num.oob + i], leafNodeIdx[j]]
            } else {
              predictions[i, oob.idx] <- tree$leafSimilarity[leafNodeIdx[j], leafNodeIdx[num.oob + i]]
            }
          }
        }
      } else {
        for (j in 1L:ntrain) {
          for (i in 1L:ntest) {
            if (leafNodeIdx[ntrain + i] >= leafNodeIdx[j]) {
              predictions[i, j] <- tree$leafSimilarity[leafNodeIdx[ntrain + i], leafNodeIdx[j]]
            } else {
              predictions[i, j] <- tree$leafSimilarity[leafNodeIdx[j], leafNodeIdx[ntrain + i]]
            }
          }
        }
      }
    }

    ### Make output matrix symmetric
    predictions[upper.tri(predictions)] <- t(predictions)[upper.tri(predictions)]
    return(predictions)
  }
