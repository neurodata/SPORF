runpredict <-
function(X, tree, comp.mode = "batch"){
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
  
  Yhats <- integer(n)
  
  if (comp.mode == "individual") {
    Xnode <- 0
    for (i in 1L:n){
      currentNode <- 1L
      while((tm <- tree$treeMap[currentNode]) > 0L){
        indexHigh <- tree$matAindex[tm + 1L]
        indexLow <- tree$matAindex[tm]
        s <- (indexHigh-indexLow)/2L
        Xnode <- sum(tree$matAstore[(indexLow+1L):indexHigh][(1:s)*2L]*X[i,tree$matAstore[(indexLow+1L):indexHigh][(1:s)*2L-1L]])
        if(Xnode <= tree$CutPoint[tm]){
          currentNode <- tree$Children[tm]
        }else{
          currentNode <- tree$Children[tm] + 1L
        }
      }
      Yhats[i] <- order(tree$ClassProb[tm*-1L, ], decreasing = T)[1L]
    }
  } else {
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
          Yhats[Assigned2Node[[m]]] <- order(tree$ClassProb[tm*-1L, ], decreasing = T)[1L]
        }
      }
      Assigned2Node[m] <-list(NULL)
    }
  }
  return(Yhats)
}
