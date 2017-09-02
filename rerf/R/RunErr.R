RunErr <-
function(X,Y,Forest, index=0L, chunk_size=0L){
  if(index && chunk_size){
    X<- X[(((index-1)*chunk_size)+1L):(index*chunk_size),,drop=FALSE]
  }
  n <- nrow(X)
  forestSize <- length(Forest)
  classProb<-double(length(Forest[[1]]$ClassProb[1,]))
  z <- integer()
  tm <- integer()
  indexHigh <- 0L
  indexLow <- 0L
  rotX<-0
  currentNode<-0L
  for(i in 1L:n){
    classProb[] <- 0
    for(j in 1L:forestSize){
      currentNode <- 1L
      
      while((tm<- Forest[[j]]$treeMap[currentNode])>0L){
        indexHigh <- Forest[[j]]$matAindex[tm+1]
        indexLow <- Forest[[j]]$matAindex[tm]
        s<-(indexHigh-indexLow)/2
        rotX<-sum(Forest[[j]]$matAstore[(indexLow+1):indexHigh][(1L:s)*2]*X[i,Forest[[j]]$matAstore[(indexLow+1):indexHigh][(1L:s)*2-1]])
        if(rotX<=Forest[[j]]$CutPoint[tm]){
          currentNode <- Forest[[j]]$Children[tm]
        }else{
          currentNode <- Forest[[j]]$Children[tm]+1
        }
      }
      classProb <- classProb + Forest[[j]]$ClassProb[tm*-1,]
    }
    z <- c(z,order(classProb,decreasing=T)[1L])
  }
  if(!index || !chunk_size){
    return(sum(z!=Y))
  }else{
    return(sum(z!=Y[(((index-1)*chunk_size)+1L):(index*chunk_size)]))
  }
}
