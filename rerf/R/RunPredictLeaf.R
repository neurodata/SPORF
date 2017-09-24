#' Calculate similarity using a single tree.
#'
#' This is the base function called by ComputeSimilarity.
#'
#' @param X an n sample by d feature matrix (preferable) or data frame which was used to train the provided forest.
#' @param tree a tree from a forest returned by RerF.
#'
#' @return similarity based on one tree
#'
#' @author James Browne (jbrowne6@jhu.edu) and Tyler Tomita (ttomita2@jhmi.edu) 
#'


RunPredictLeaf <-
    function(X, tree){
        tm <- 0L
        currentNode<-0L
        curr_ind <- 0L
        classProb<-double(length(tree$ClassProb[1L,]))
        num_classes <- ncol(tree$ClassProb)
        n <- nrow(X)

        # do we need to rotate the data?
        if (!is.null(tree$rotmat)) {
            if (is.null(tree$rotdims)) {
                X[] <- X%*%tree$rotmat
            } else {
                X[, tree$rotdims] <- X[, tree$rotdims]%*%tree$rotmat
            }
        }

        leafIdx <- integer(n)
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
                    Xnode[1L:nodeSize] <- X[Assigned2Node[[m]],tree$matAstore[indexLow:indexHigh][(1L:s)*2L-1L], drop = F]%*%
                        tree$matAstore[indexLow:indexHigh][(1L:s)*2L]
                    moveLeft <- Xnode[1L:nodeSize] <= tree$CutPoint[tm]
                    Assigned2Node[[tm*2L]] <- Assigned2Node[[m]][moveLeft]
                    Assigned2Node[[tm*2L + 1L]] <- Assigned2Node[[m]][!moveLeft]
                } else {
                    leafIdx[Assigned2Node[[m]]] <- tm*-1L
                }
            }
            Assigned2Node[[m]]<-NA
        }
        return(leafIdx)
    }
