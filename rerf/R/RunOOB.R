#' Predict class labels on out-of-bag observations using a single tree.
#'
#' This is the base function called by OOBPredict.
#'
#' @param X an n sample by d feature matrix (preferable) or data frame which was used to train the provided forest.
#' @param tree a tree from a forest returned by RerF.
#'
#' @return predictions
#'
#' @author James and Tyler, jbrowne6@jhu.edu and ttomita2@jhmi.edu
#'

RunOOB <-
function(X, tree) {
  X <- X[tree$ind, , drop = F]
  currentNode<-0L
  curr_ind <- 0L
  tm <- 0L
  classProb<-double(length(tree$ClassProb[1,]))
  n <- nrow(X)
  predictions <- integer(n)
  
  # do we need to rotate the data?
  if (!is.null(tree$rotmat)) {
    if (is.null(tree$rotdims)) {
      X[] <- X%*%tree$rotmat
    } else {
      X[, tree$rotdims] <- X[, tree$rotdims]%*%tree$rotmat
    }
  }
  
  # if (comp.mode == "individual") {
  #   curr_ind <- 1L
  #   Xnode <- 0
  #   for (i in 1:n){
  #     currentNode <- 1L
  #     while((tm <- tree$treeMap[currentNode]) > 0L){
  #       indexHigh <- tree$matAindex[tm + 1L]
  #       indexLow <- tree$matAindex[tm]
  #       s <- (indexHigh-indexLow)/2L
  #       Xnode <- sum(tree$matAstore[(indexLow+1L):indexHigh][(1:s)*2L]*X[i,tree$matAstore[(indexLow+1L):indexHigh][(1:s)*2L-1L]])
  #       if(Xnode <= tree$CutPoint[tm]){
  #         currentNode <- tree$Children[tm]
  #       }else{
  #         currentNode <- tree$Children[tm] + 1L
  #       }
  #     }
  #     predictions[i] <- order(tree$ClassProb[tm*-1L, ], decreasing = T)[1L]
  #   }
  
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
        Assigned2Node[[tree$Children[tm]]] <- Assigned2Node[[m]][moveLeft]
        Assigned2Node[[tree$Children[tm] + 1L]] <- Assigned2Node[[m]][!moveLeft]
      } else {
        predictions[Assigned2Node[[m]]] <- order(tree$ClassProb[tm*-1L, ], decreasing = T)[1L]
      }
    }
    Assigned2Node[m] <-list(NULL)
  }
  
  return(predictions)
}
