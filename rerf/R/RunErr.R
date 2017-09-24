#' Determine Error Rate of Forest
#'
#' This function is no longer used.  It was the workhorse function for Error Predict. 
#'
#' @param X an n by d numeric matrix (preferable) or data frame. The rows correspond to observations and columns correspond to features of a test set, which should be different from the training set.
#' @param forest a forest trained using the RerF function.
#' @param num.cores the number of cores to use while training. If NumCores=0 then 1 less than the number of cores reported by the OS are used. (NumCores=0)
#' @param Xtrain  an n by d numeric matrix (preferable) or data frame. This should be the same data matrix/frame used to train the forest, and is only required if RerF was called with rank.transform = TRUE. (Xtrain=NULL)
#' @param aggregate.output if TRUE then the tree predictions are aggregated via majority vote. Otherwise, the individual tree predictions are returned. (aggregate.output=TRUE)
#' @param output.scores if TRUE then predicted class scores (probabilities) for each observation are returned rather than class labels. (output.scores = FALSE)
#'
#' @return predictions an n length vector of predictions
#'
#' @author James Browne (jbrowne6@jhu.edu) and Tyler Tomita (ttomita2@jhmi.edu) 
#
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
