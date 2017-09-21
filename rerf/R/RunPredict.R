#' Predict class labels on a test set using a single tree.
#'
#' This is the base function called by Predict.
#'
#' @param X an n sample by d feature matrix (preferable) or data frame which was used to train the provided forest.
#' @param tree a tree from a forest returned by RerF.
#'
#' @return predictions an n length vector of prediction based on the tree provided to this function
#'
#' @author James Browne and Tyler Tomita, jbrowne6@jhu.edu and ttomita2@jhmi.edu 
#'

RunPredict <-
function(X, tree){
  tm <- 0L
  currentNode<-0L
  curr_ind <- 0L
  classProb<-double(length(tree$ClassProb[1,]))
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
  
  predictions <- integer(n)
  
  Xnode <- double(n)
  numNodes <- length(tree$treeMap)
  Assigned2Node <- vector("list", numNodes)
  Assigned2Node[[1L]] <- 1:n
  for (m in 1:numNodes) {
    nodeSize <- length(Assigned2Node[[m]])
    if (nodeSize > 0L) {
      if ((tm <- tree$treeMap[m]) > 0L) {
        indexHigh <- tree$matAindex[tm+1L]
        indexLow <- tree$matAindex[tm] + 1L
        s <- (indexHigh - indexLow + 1L)/2L
        Xnode[1:nodeSize] <- X[Assigned2Node[[m]],tree$matAstore[indexLow:indexHigh][(1:s)*2L-1L], drop = F]%*%
          tree$matAstore[indexLow:indexHigh][(1:s)*2L]
        moveLeft <- Xnode[1:nodeSize] <= tree$CutPoint[tm]
        Assigned2Node[[tm*2L]] <- Assigned2Node[[m]][moveLeft]
        Assigned2Node[[tm*2L+1L]] <- Assigned2Node[[m]][!moveLeft]
      } else {
        predictions[Assigned2Node[[m]]] <- order(tree$ClassProb[tm*-1L, ], decreasing = T)[1L]
      }
    }
    Assigned2Node[m] <-list(NULL)
  }
  return(predictions)
}
