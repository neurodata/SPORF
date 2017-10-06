#' Predict class labels on out-of-bag observations using a single tree.
#'
#' This is the base function called by OOBPredict.
#'
#' @param X an n sample by d feature matrix (preferable) or data frame which was used to train the provided forest.
#' @param tree a tree from a forest returned by RerF.
#'
#' @return predictions prediction matrix used by OOBPredict
#'

RunOOB <-
    function(X, tree) {
        X <- X[tree$ind, , drop = F]

        # do we need to rotate the data?
        if (!is.null(tree$rotmat)) {
            if (is.null(tree$rotdims)) {
                X[] <- X%*%tree$rotmat
            } else {
                X[, tree$rotdims] <- X[, tree$rotdims]%*%tree$rotmat
            }
        }

        currentNode<-0L
        curr_ind <- 0L
        tm <- 0L
        n <- nrow(X)
        predictions <- integer(n)

        Xnode <- double(n)
        numNodes <- length(tree$treeMap)
        Assigned2Node <- vector("list", numNodes)
        Assigned2Node[[1L]] <- 1L:n
        for (m in 1L:numNodes) {
            nodeSize <- length(Assigned2Node[[m]])
            if (nodeSize > 0L) {
                if ((tm <- tree$treeMap[m]) > 0L) {
                    indexHigh <- tree$matAindex[tm+1L]
                    indexLow <- tree$matAindex[tm] + 1L
                    s <- (indexHigh - indexLow + 1L)/2L
                    Xnode[1:nodeSize] <- X[Assigned2Node[[m]],tree$matAstore[indexLow:indexHigh][(1L:s)*2L-1L], drop = F]%*%
                        tree$matAstore[indexLow:indexHigh][(1L:s)*2L]
                    moveLeft <- Xnode[1L:nodeSize] <= tree$CutPoint[tm]
                    Assigned2Node[[tm*2L]] <- Assigned2Node[[m]][moveLeft]
                    Assigned2Node[[tm*2L + 1L]] <- Assigned2Node[[m]][!moveLeft]
                } else {
                    predictions[Assigned2Node[[m]]] <- which.max(tree$ClassProb[tm*-1L, ])
                }
            }
            Assigned2Node[[m]]<-NA
        }
        return(predictions)
    }
