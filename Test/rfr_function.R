require(Rcpp)
require(RcppArmadillo)
sourceCpp("split.cpp")

build.tree <- function(X, Y, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB, CNS, Progress, rotate){
  #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  # rfr builds a randomer classification forest structure made up of a list
  # of trees.  This forest is randomer because each node is rotated before 
  # being split (as described by Tyler Tomita).  Each tree is made of nodes
  # which are numbered from left to right within a particular level (depth)
  # of a tree.  The loop over nodes when considering splits is made in the
  # same order as the numbering of the nodes. That is, nodes are traversed
  # across a single level from left to right then moves to the left-most node
  # of the next level.
  #
  #  INPUT:
  #
  # X is an n-by-p matrix, where rows represent observations and columns
  # represent features
  #
  # Y is an n-by-1 array of integer class labels. Unique class labels
  # must be contiguous and start from 1 (e.g. [0,1,2] is not okay;
  # neither is [1,3,4])
  #
  # MinParent is an integer specifying the minimum number of observations
  # a node must have in order for an attempt to split to be made.  Lower
  # values may lead to overtraining and increased training time.
  #
  # trees is the number of trees that will be in the forest.
  #
  # MaxDepth is the maximum depth that a tree can grow to.  If set to "inf"
  # then there is no maximum depth.  If set to 0 then a maximum depth is
  # calculated based on the number of classes and number of samples provided.
  #
  # bagging is the percentage of training data to withhold during each
  # training iteration.  If set to 0 then the entire training set is used
  # during every iteration.  The withheld portion of the training data
  # is used to calculate OOB error for the tree.
  #
  # ClassCt is the number of different classes in Y.  It is calculated 
  # in the calling function to prevent recalculation by each forked function 
  # when in parallel.
  #
  # FUN is the function used to create the projection matrix.  The matrix
  # returned by this function should be a p-by-u matrix where p is the
  # number of columns in the input matrix X and u is any integer > 0.
  # u can also vary from node to node.
  #
  # options is a list of inputs to the user provided projection matrix
  # creation function -- FUN.
  #
  # rotate is a boolean specifying whether or not to randomly rotate the
  # for each tree. If TRUE, then a different random rotation will be applied
  # to each bagged subsample prior to building each tree. If the number of
  # dimensions is greater than 1000, then a random subset of 1000 of the
  # dimensions will be rotated and the others will be left alone
  #
  # COOB is a boolean that determines whether or not OOB error is calculated.
  # If bagging equals zero then COOB is ignored.  If bagging does not equal 
  # zero and COOB is TRUE then OOB is calculated and printed to the screen.
  # 
  # CNS is a boolean that specifies whether to store the node size of each
  # node.
  #
  # Progress is a boolean.  When true a progress marker is printed to the 
  # screen every time a tree is grown.  This is useful for large input.
  #
  # OUTPUT:
  #
  # A forest construct made up of trees.  This forest can be used to make 
  # predictions on new inputs.  When COOB=TRUE then the output is a list
  # containing $forest and $OOBmat.  $forest is the forest structure and
  # OOBmat is the OOB error for each tree.
  #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  
  #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  # Predefine variables to prevent recreation during loops
  #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  nClasses <- length(classCt)
  ret <- list(MaxDeltaI = 0, BestVar = 0L, BestSplitIdx = 0L)
  currIN <- 0L
  currLN <- 0L
  w <- nrow(X)
  p <- ncol(X)
  perBag <- (1-bagging)*w
  Xnode<-double(w) # allocate space to store the current projection
  SortIdx<-integer(w) 
  x <- double(w)
  y <- integer(w)
  # if(object.size(X) > 1000000){
  #   OS<-TRUE
  # }else{
  #   OS<-FALSE
  # }
  
  # Calculate the Max Depth and the max number of possible nodes
  if(MaxDepth == "inf"){
    MaxNumNodes <- 2L*w # number of tree nodes for space reservation
  }else{
    if(MaxDepth==0){
      MaxDepth <- ceiling((log2(w)+log2(nClasses))/2)
    }
    MaxNumNodes <- 2L^(MaxDepth+1L)  # number of tree nodes for space reservation
  }
  
  maxIN <- ceiling(MaxNumNodes/2)
  treeMap <- integer(MaxNumNodes)
  ClassProb <- matrix(data = 0, nrow = maxIN, ncol = nClasses)
  CutPoint <- double(maxIN)
  Children <- integer(maxIN)
  NdSize <- integer(MaxNumNodes)
  NDepth <- integer(MaxNumNodes)
  Assigned2Node<- vector("list",MaxNumNodes) 
  ind <- double(w)
  #Matrix A storage variables
  matAindex <- integer(maxIN)
  matAsize <- ceiling(w/2)
  matAstore <- integer(matAsize)
  matAindex[1L] <- 0L
  #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  #                            Start tree creation
  #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  # rotate the data?
  if (rotate) {
    # if p > 1000 then rotate only a random subset of 1000 of the dimensions
    if (p > 1000L) {
      rotmat <- rrot(1000L)
      rotdims <- sample.int(p, 1000L)
      X[, rotdims] <- X[, rotdims]%*%rotmat
    } else {
      rotmat <- rrot(p)
      X <- X%*%rotmat
    }
  }
  
  # intialize values for new tree before processing nodes
  #NDepth[]<- 0L #delete this?
  NDepth[1L]<-1L
  CurrentNode <- 1L
  NextUnusedNode <- 2L
  NodeStack <- 1L
  highestParent <- 1L
  ind[] <- 0L
  currIN <- 0L
  currLN <- 0L
  # Determine bagging set 
  # Assigned2Node is the set of row indices of X assigned to current node
  if(bagging != 0){
    if(replacement){
      go <- T
      while (go) {
        if(stratify){
          ind[1:classCt[1]]<-sample(Cindex[[1]], classCt[1], replace=TRUE)
          for(z in 2:nClasses){
            ind[(classCt[z-1]+1):classCt[z]]<- sample(Cindex[[z]], classCt[z]-classCt[z-1], replace=TRUE)
          }
        }else{
          ind<-sample(1:w, w, replace=TRUE)
        }
        go <- all(1:w %in% ind)
      }
      Assigned2Node[[1]] <- ind
    }else{
      ind[1:perBag] <- sample(1:w, perBag, replace = FALSE)
      Assigned2Node[[1]] <- ind[1:perBag]        
    }
  }else{
    Assigned2Node[[1]] <- 1:w        
  }
  
  # main loop over nodes
  while (CurrentNode < NextUnusedNode){
    # determine working samples for current node.
    NodeRows <- Assigned2Node[CurrentNode] 
    Assigned2Node[[CurrentNode]]<-NA #remove saved indexes
    NdSize[CurrentNode] <- length(NodeRows[[1L]]) #determine node size
    # determine number of samples in current node then
    # determine their percentages in the node
    ClassCounts <- tabulate(Y[NodeRows[[1L]]], nClasses)
    ClProb <- ClassCounts/NdSize[CurrentNode]
    # compute impurity for current node
    I <- sum(ClassCounts*(1 - ClProb))
    # if node is impure and large enough then attempt to find good split
    if (NdSize[CurrentNode] < MinParent || I <= 0 || NDepth[CurrentNode]==MaxDepth){
      treeMap[CurrentNode] <- currLN <- currLN - 1L
      ClassProb[currLN*-1,] <- ClProb
      NodeStack <- NodeStack[-1L]
      CurrentNode <- NodeStack[1L]
      if(is.na(CurrentNode)){
        break
      }
      next 
    }
    
    # create projection matrix (sparseM) by calling the custom function FUN
    sparseM <- FUN(options)
    nnz <- nrow(sparseM)
    # Check each projection to determine which splits the best.
    ret$MaxDeltaI <- 0
    # ret$BestVar <- 0L
    # ret$BestSplitIdx <- 0L
    nz.idx <- 1L
    
    while (nz.idx <= nnz) {
      # for(q in unique(sparseM[,2])){
      # Parse sparseM to the column of the projection matrix at this iteration
      feature.idx <- sparseM[nz.idx, 2L]
      feature.nnz <- 0L
      while(sparseM[nz.idx + feature.nnz, 2L] == feature.idx) {
        feature.nnz <- feature.nnz + 1L
        if (nz.idx + feature.nnz > nnz) {
          break
        }
      }
      lrows <- nz.idx:(nz.idx + feature.nnz - 1L)
      
      #Project input into new space
      Xnode[1:NdSize[CurrentNode]] <- X[NodeRows[[1L]],sparseM[lrows,1], drop=FALSE]%*%sparseM[lrows,3, drop=FALSE]
      
      #Sort the projection, Xnode, and rearrange Y accordingly
      SortIdx[1:NdSize[CurrentNode]] <- order(Xnode[1:NdSize[CurrentNode]])
      x[1:NdSize[CurrentNode]] <- Xnode[SortIdx[1:NdSize[CurrentNode]]]
      y[1:NdSize[CurrentNode]] <- Y[NodeRows[[1L]]][SortIdx[1:NdSize[CurrentNode]]]
      
      ##################################################################
      #                    Find Best Split
      ##################################################################
      
      ret[] <- findSplit(x = x[1:NdSize[CurrentNode]], y = y[1:NdSize[CurrentNode]], ndSize = NdSize[CurrentNode], I = I,
                         maxdI = ret$MaxDeltaI, bv = ret$BestVar, bs = ret$BestSplitIdx, nzidx = nz.idx, cc = ClassCounts)
      
      nz.idx <- nz.idx + feature.nnz
    }
    
    if (ret$MaxDeltaI == 0) {
      treeMap[CurrentNode] <- currLN <- currLN - 1L
      ClassProb[currLN*-1,] <- ClProb
      NodeStack <- NodeStack[-1L]
      CurrentNode <- NodeStack[1L]
      if(is.na(CurrentNode)){
        break
      }
      next
    }
    
    # Recalculate the best projection
    feature.idx <- sparseM[ret$BestVar, 2L]
    feature.nnz <- 0L
    # print(BestVar.startidx)
    # print(feature.idx)
    while(sparseM[ret$BestVar + feature.nnz, 2L] == feature.idx) {
      feature.nnz <- feature.nnz + 1L
      if (ret$BestVar + feature.nnz > nnz) {
        break
      }
    }
    lrows <- ret$BestVar:(ret$BestVar + feature.nnz - 1L)
    Xnode[1:NdSize[CurrentNode]]<-X[NodeRows[[1L]],sparseM[lrows,1], drop=FALSE]%*%sparseM[lrows,3, drop=FALSE]
    
    # reorder the projection and find the cut value
    SortIdx[1:NdSize[CurrentNode]] <- order(Xnode[1:NdSize[CurrentNode]] )
    # determine split value as mean of values on either side of split
    BestSplitValue <- sum(Xnode[SortIdx[ret$BestSplitIdx:(ret$BestSplitIdx+1L)]])/2
    
    # find which child node each sample will go to and move
    # them accordingly
    MoveLeft <- Xnode[1:NdSize[CurrentNode]]  <= BestSplitValue
    
    # Move samples left or right based on split
    Assigned2Node[[NextUnusedNode]] <- NodeRows[[1L]][MoveLeft]
    Assigned2Node[[NextUnusedNode+1L]] <- NodeRows[[1L]][!MoveLeft]
    
    # highestParent keeps track of the highest needed matrix and cutpoint
    # this reduces what is stored in the forest structure
    if(CurrentNode>highestParent){
      highestParent <- CurrentNode
    }
    # Determine children nodes and their attributes
    treeMap[CurrentNode] <- currIN <- currIN + 1L
    Children[currIN] <- NextUnusedNode
    NDepth[NextUnusedNode]=NDepth[CurrentNode]+1L
    NDepth[NextUnusedNode+1L]=NDepth[CurrentNode]+1L
    # Pop the current node off the node stack
    # this allows for a breadth first traversal
    NodeStack <- NodeStack[-1L]
    NodeStack <- c(NextUnusedNode, NextUnusedNode+1L, NodeStack)
    NextUnusedNode <- NextUnusedNode + 2L
    # Store the projection matrix for the best split
    currMatAlength <- length(sparseM[lrows,c(1,3)])
    if(matAindex[currIN] + currMatAlength > matAsize){ #grow the vector when needed.
      matAsize <- matAsize*2
      matAstore[matAsize] <- 0L
    }
    if (options[[3]] != "frc" && options[[3]] != "continuous") {
      matAstore[(matAindex[currIN]+1):(matAindex[currIN]+currMatAlength)] <- as.integer(t(sparseM[lrows,c(1,3)]))
    } else {
      matAstore[(matAindex[currIN]+1):(matAindex[currIN]+currMatAlength)] <- t(sparseM[lrows,c(1,3)])
    }
    matAindex[currIN+1] <- matAindex[currIN]+currMatAlength 
    CutPoint[currIN] <- BestSplitValue
    
    # Store ClassProbs for this node.
    # Only really useful for leaf nodes, but could be used instead of recalculating
    # at each node which is how it is currently.
    CurrentNode <- NodeStack[1L]
    if(is.na(CurrentNode)){
      break
    }
  }
  
  # If input is large then garbage collect prior to adding onto the forest structure.
  # if(OS){
  #   gc()
  # }
  
  # save current tree structure to the forest
  
  currLN <- currLN * -1
  if (rotate) {
    if (p > 1000L) {
      if (CNS) {
        if(bagging!=0 && COOB){
          tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                       "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "ind"=which(!(1:w %in% ind)),
                       "NdSize" = NdSize[1L:(NextUnusedNode-1L)], "rotmat" = rotmat, "rotdims" = rotdims)
        }else{
          tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                       "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "NdSize" = NdSize[1L:(NextUnusedNode-1L)],
                       "rotmat" = rotmat, "rotdims" = rotdims)
        }
      } else {
        if(bagging!=0 && COOB){
          tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                       "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "ind"=which(!(1:w %in% ind)),
                       "rotmat" = rotmat, "rotdims" = rotdims)
        }else{
          tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                       "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "rotmat" = rotmat, "rotdims" = rotdims)
        }
      }
    } else {
      if (CNS) {
        if(bagging!=0 && COOB){
          tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                       "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "ind"=which(!(1:w %in% ind)),
                       "NdSize" = NdSize[1L:(NextUnusedNode-1L)], "rotmat" = rotmat)
        }else{
          tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                       "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "NdSize" = NdSize[1L:(NextUnusedNode-1L)],
                       "rotmat" = rotmat)
        }
      } else {
        if(bagging!=0 && COOB){
          tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                       "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "ind"=which(!(1:w %in% ind)),
                       "rotmat" = rotmat)
        }else{
          tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                       "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "rotmat" = rotmat)
        }
      }
    }
  } else {
    if (CNS) {
      if(bagging!=0 && COOB){
        tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                     "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "ind"=which(!(1:w %in% ind)), "NdSize" = NdSize[1L:(NextUnusedNode-1L)])
      }else{
        tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                     "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "NdSize" = NdSize[1L:(NextUnusedNode-1L)])
      }
    } else {
      if(bagging!=0 && COOB){
        tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                     "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "ind"=which(!(1:w %in% ind)))
      }else{
        tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                     "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)])
      }
    }
  }
  
  if(Progress){
    cat("|")
  }
  return(tree)
}

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#                      Default option to make projection matrix 
#
# this is the randomer part of random forest. The sparseM 
# matrix is the projection matrix.  The creation of this
# matrix can be changed, but the nrow of sparseM should
# remain p.  The ncol of the sparseM matrix is currently
# set to mtry but this can actually be any integer > 1;
# can even greater than p.
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
makeA <- function(options){
  p <- options[[1L]]
  d <- options[[2L]]
  method <- options[[3L]]
  if(method == 1L){
    rho<-options[[4L]]
    nnzs <- round(p*d*rho)
    sparseM <- matrix(0L, nrow=p, ncol=d)
    sparseM[sample(1L:(p*d),nnzs, replace=F)]<-sample(c(1L,-1L),nnzs,replace=T)
  }
  #The below returns a matrix after removing zero columns in sparseM.
  ind<- which(sparseM!=0,arr.ind=TRUE)
  return(cbind(ind,sparseM[ind]))        
}


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#                       Calculate Error Rate
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
RunErr <- function(X,Y,Forest, index=0L, chunk_size=0L){
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


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#                       Calculate OOB Error Rate
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
runerrOOB <- function(X, tree, comp.mode = "batch") {
  X <- X[tree$ind, ]
  currentNode<-0L
  curr_ind <- 0L
  tm <- 0L
  classProb<-double(length(tree$ClassProb[1,]))
  n <- nrow(X)
  Yhats <- integer(n)
  
  # do we need to rotate the data?
  if (!is.null(tree$rotmat)) {
    if (is.null(tree$rotdims)) {
      X <- X%*%tree$rotmat
    } else {
      X[, tree$rotdims] <- X[, tree$rotdims]%*%tree$rotmat
    }
  }
  
  if (comp.mode == "individual") {
    curr_ind <- 1L
    Xnode <- 0
    for (i in 1:n){
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

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#                          Make Predictions
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
runpredict <- function(X, tree, comp.mode = "batch"){
  tm <- 0L
  currentNode<-0L
  curr_ind <- 0L
  classProb<-double(length(tree$ClassProb[1,]))
  num_classes <- ncol(tree$ClassProb)
  n <- nrow(X)
  
  # do we need to rotate the data?
  if (!is.null(tree$rotmat)) {
    if (is.null(tree$rotdims)) {
      X <- X%*%tree$rotmat
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


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#                         OOB Error as tree grows 
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
errgrow <- function(Y,probmat){
  z<-integer(length(Y))
  forestSize <- length(probmat) - 1L
  numClass <- ncol(probmat[[1L]])-2
  probcurrent<- matrix(data = 0, nrow = length(Y), ncol = numClass)
  numWrong<- 0L
  numTotal<- 0L
  for(q in 1L:forestSize){
    for(j in 1L:nrow(probmat[[q]])){
      probcurrent[probmat[[q]][j,1L], ] <- probcurrent[probmat[[q]][j,1L], ]+probmat[[q]][j,3:(2+numClass)]
    }
    numWrong<- 0L
    numTotal<- 0L
    for(m in 1L:length(Y)){
      if(any(probcurrent[m,]!=0)){
        if(order(probcurrent[m,],decreasing=T)[1L]!=Y[m]){
          numWrong <- numWrong+1L
        }
        numTotal<-numTotal+1L
      }
    }
    z[q] <- numWrong/numTotal
  }
  return(z)
}

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#   Run R-Rerf byte compiled and parallel                       
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
rerf <- function(X, Y, MinParent=6L, trees=100L, MaxDepth=0L, bagging = .2, replacement=TRUE, stratify=FALSE, FUN=makeA, options=c(ncol(X), round(ncol(X)^.5),1L, 1/ncol(X)), rank.transform = FALSE, COOB=FALSE, CNS=FALSE, Progress=FALSE, rotate = F, NumCores=0L, seed = 1L){
  
  #keep from making copies of X
  X <- as.matrix(X)
  if (rank.transform) {
    X <- rank.matrix(X)
  }
  if(!is.integer(Y)){
    Y <- as.integer(Y)
  }
  uY<-length(unique(Y))
  classCt <- tabulate(Y,uY)
  for(q in 2:uY){
    classCt[q] <- classCt[q]+classCt[q-1]
  }
  if(stratify){
    Cindex<-vector("list",uY)
    for(m in 1L:uY){
      Cindex[[m]]<-which(Y==m)
    }
  }else{
    Cindex<-NULL
  }
  
  if (!require(compiler)){
    cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to create the forest.\n")
    comp_tree <<- build.tree
  }
  if(!exists("comp_tree")){
    setCompilerOptions("optimize"=3)
    comp_tree <<- cmpfun(build.tree)
  }
  
  if (NumCores!=1L){
    if(require(parallel)){
      RNGkind("L'Ecuyer-CMRG")
      set.seed(seed)
      mc.reset.stream()
      if(NumCores==0){
        #Use all but 1 core if NumCores=0.
        NumCores=detectCores()-1L
      }
      NumCores=min(NumCores,trees)
      mcrun<- function(...) comp_tree(X, Y, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB=COOB, CNS=CNS, Progress=Progress, rotate)
      gc()
      forest <- mclapply(1:trees, mcrun, mc.cores = NumCores, mc.set.seed=TRUE)
    }else{
      #Parallel package not available.
      cat("Package 'parallel' not available.\nExecution will continue without parallelization.\nThis will increase the time required to create the forest\n")
      if (!require(compiler)) {
        cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to create the forest.\n")
        comp_forest <<- function(X, Y, trees, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB, CNS, Progress, rotate, seed) {
          set.seed(seed)
          return(lapply(1:trees, comp_tree(X, Y, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB=COOB, CNS=CNS, Progress=Progress, rotate=rotate)))
        }
      }
      if (!exists("comp_forest")) {
        build.forest <<- function(X, Y, trees, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB, CNS, Progress, rotate, seed) {
          set.seed(seed)
          forest <- vector("list", trees)
          for (t in 1:trees) {
            forest[[t]] <- comp_tree(X, Y, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB=COOB, CNS=CNS, Progress=Progress, rotate=rotate)
          }
          return(forest)
        }
        comp_forest <<- cmpfun(build.forest)
      }
      forest <- comp_forest(X, Y, trees, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB, CNS, Progress, rotate, seed)
    }
  }else{
    #Use just one core.
    if (!require(compiler)) {
      cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to create the forest.\n")
      comp_forest <<- function(X, Y, trees, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB, CNS, Progress, rotate, seed) {
        set.seed(seed)
        return(lapply(1:trees, comp_tree(X, Y, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB=COOB, CNS=CNS, Progress=Progress, rotate=rotate)))
      }
    }
    if (!exists("comp_forest")) {
      build.forest <- function(X, Y, trees, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB, CNS, Progress, rotate, seed) {
        set.seed(seed)
        forest <- vector("list", trees)
        for (t in 1:trees) {
          forest[[t]] <- comp_tree(X, Y, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB=COOB, CNS=CNS, Progress=Progress, rotate=rotate)
        }
        return(forest)
      }
      comp_forest <<- cmpfun(build.forest)
    }
    forest <- comp_forest(X, Y, trees, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB, CNS, Progress, rotate, seed)
  }
  if(Progress){
    cat("\n\n")
  }
  
  return(forest)
}

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#                      Run OOB Error rate byte compiled and parallel 
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
OOBpredict <- function(X, Forest, NumCores=0, rank.transform = F, comp.mode = "batch"){
  if (!is.matrix(X)) {
    X <- as.matrix(X)
  }
  if (rank.transform){
    X <- rank.matrix(X)
  }
  n <- nrow(X)
  
  if(!require(compiler)){
    cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to find the OOB error rate.\n")
    comp_errOOB <<- runerrOOB
  }
  
  if(!exists("comp_errOOB")){
    setCompilerOptions("optimize"=3)
    comp_errOOB <<- cmpfun(runerrOOB)
  } 
  comp_errOOB_caller <- function(tree, ...) comp_errOOB(X = X, tree = tree, comp.mode = comp.mode)
  
  f_size <- length(Forest)
  if(NumCores!=1){
    if(require(doParallel)){
      if(NumCores==0){
        #Use all but 1 core if NumCores=0.
        NumCores=detectCores()-1L
      }
      #Start mclapply with NumCores Cores.
      NumCores <- min(NumCores, f_size)
      gc()
      if ((object.size(Forest) > 2e9) | (object.size(X) > 2e9)) {
        cl <<- makeCluster(spec = NumCores, type = "PSOCK")
        clusterExport(cl = cl, varlist = c("X", "comp.mode", "comp_errOOB"), envir = environment())
        Yhats <- parLapply(cl = cl, Forest, fun = comp_errOOB_caller)
      } else {
        cl <<- makeCluster(spec = NumCores, type = "FORK")
        Yhats <- parLapply(cl = cl, Forest, fun = comp_errOOB_caller)
      }
      stopCluster(cl)
    }else{
      #Parallel package not available.
      cat("Package 'parallel' not available.\nExecution will continue without parallelization.\nThis will increase the time required to create the forest\n")
      Yhats <- lapply(Forest, FUN = comp_errOOB_caller)
    }
  }else{
    #Use just one core.
    Yhats <- lapply(Forest, FUN = comp_errOOB_caller)
  }
  
  num_classes <- ncol(Forest[[1]]$ClassProb)
  # Have to make the last entry before this bottom will work.
  scores <- matrix(0,nrow=n, ncol=num_classes)
  oobCounts <- vector(mode = "integer", length = n)
  for(m in 1:f_size){
    oobCounts[Forest[[m]]$ind] <- oobCounts[Forest[[m]]$ind] + 1L
    # scores[Forest[[m]]$ind] <- scores[Forest[[m]]$ind] + Yhats[[m]]
    for(k in 1:length(Forest[[m]]$ind)){
      scores[Forest[[m]]$ind[k], Yhats[[m]][k]] <- scores[Forest[[m]]$ind[k], Yhats[[m]][k]] + 1
    }
  }
  has.counts <- oobCounts != 0L
  scores[has.counts, ] <- scores[has.counts, ]/oobCounts[has.counts]
  return(scores)
}

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#                      Run predict byte compiled and parallel 
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
predict <- function(X, Forest, NumCores=0, rank.transform = F, Xtrain = NULL, comp.mode = "batch"){
  if (!is.matrix(X)) {
    X <- as.matrix(X)
  }
  if (rank.transform) {
    if (is.null(Xtrain)) {
      ############ error ############
    } else {
      X <- rank.interpolate(Xtrain, X)
    }
  }
  
  if(!require(compiler)){
    cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to predict.\n")
    comp_predict <<- runpredict
  }
  
  if(!exists("comp_predict")){
    setCompilerOptions("optimize"=3)
    comp_predict <<- cmpfun(runpredict)
  } 
  
  comp_predict_caller <- function(tree, ...) comp_predict(X=X, tree=tree, comp.mode = comp.mode)
  
  f_size <- length(Forest)
  if(NumCores!=1){
    if(require(parallel)){
      if(NumCores==0){
        #Use all but 1 core if NumCores=0.
        NumCores=detectCores()-1L
      }
      #Start mclapply with NumCores Cores.
      NumCores <- min(NumCores, f_size)
      gc()
      if ((object.size(Forest) > 2e9) | (object.size(X) > 2e9)) {
        cl <- makeCluster(spec = NumCores, type = "PSOCK")
        clusterExport(cl = cl, varlist = c("X", "comp.mode", "comp_predict"), envir = environment())
        Yhats <- parLapply(cl = cl, Forest, fun = comp_predict_caller)
      } else {
        cl <- makeCluster(spec = NumCores, type = "FORK")
        Yhats <- parLapply(cl = cl, Forest, fun = comp_predict_caller)
      }
      stopCluster(cl)
    }else{
      #Parallel package not available.
      cat("Package 'parallel' not available.\nExecution will continue without parallelization.\nThis will increase the time required to predict.\n")
      Yhats <- lapply(Forest, FUN = comp_predict_caller)
    }
  }else{
    #Use just one core.
    Yhats <- lapply(Forest, FUN = comp_predict_caller)
  }
  
  num_classes <- ncol(Forest[[1]]$ClassProb)
  scores <- matrix(0,nrow=nrow(X), ncol=num_classes)
  for(m in 1:f_size){
    scores <- scores + Yhats[[m]]
    for(k in 1:nrow(X)){
      scores[k, Yhats[[m]][k]] <- scores[k, Yhats[[m]][k]] + 1
    }
  }
  scores <- scores/f_size
  return(scores)
}

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#                      Run Error rate byte compiled and parallel 
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
error_rate <- function(X,Y,Forest, NumCores=0L){
  if(!is.null(Forest$forest)){
    Forest<-Forest$forest
  }
  if(!require(compiler)){
    cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to find the error rate.\n")
    comp_err <<- RunErr
  }
  
  if(!exists("comp_err")){
    setCompilerOptions("optimize"=3)
    comp_err <<- cmpfun(RunErr)
  } 
  
  X<- as.matrix(X)
  if(NumCores!=1){
    if(require(parallel)){
      if(NumCores==0){
        #Use all but 1 core if NumCores=0.
        NumCores=detectCores()-1L
      }
      #Start mclapply with NumCores Cores.
      if (nrow(X)%%NumCores==0){
        chunk_size <- nrow(X)/NumCores
        comp_err_caller <- function(z, ...) comp_err(X=X,Y=Y,Forest=Forest,index=z, chunk_size=chunk_size)
        gc()
        total_misclassified <- sum(as.numeric(mclapply(1L:NumCores,comp_err_caller, mc.cores=NumCores)))
      }else{
        if(nrow(X) > NumCores){
          chunk_size <- floor(nrow(X)/NumCores)
          comp_err_caller <- function(z, ...) comp_err(X=X,Y=Y,Forest=Forest,index=z, chunk_size=chunk_size)
          gc()
          total_misclassified <- sum(as.numeric(mclapply(1L:NumCores,comp_err_caller, mc.cores=NumCores)))
        }
        comp_err_caller <- function(z, ...) comp_err(X=X[(NumCores*chunk_size+1L):nrow(X),,drop=FALSE],Y=Y[(NumCores*chunk_size+1L):nrow(X)],Forest=Forest,index=z, chunk_size=1L)
        gc()
        total_misclassified <- total_misclassified + sum(as.numeric(mclapply(1L:(nrow(X)%%NumCores), comp_err_caller, mc.cores=(nrow(X)%%NumCores))))
      }
    }else{
      #Parallel package not available.
      cat("Package 'parallel' not available.\nExecution will continue without parallelization.\nThis will increase the time required to create the forest\n")
      total_misclassified <-comp_err(X, Y, Forest)
    }
  }else{
    #Use just one core.
    total_misclassified <-comp_err(X, Y, Forest)
  }
  return(total_misclassified/nrow(X))
}

skip.streams <- function(n) {
  x <- .Random.seed
  for (i in seq_len(n))
    x <- nextRNGStream(x)
  assign('.Random.seed', x, pos=.GlobalEnv)
}

# function for finding the indices where a label change occurs in a sorted Y vector
groupidx <- function(Y) {
  n <- length(Y)
  class.start.idx <- vector("integer", Y[n])
  y <- 1L
  class.start.idx[1L] <- 1L
  for (i in 2:n) {
    if (Y[i] != y) {
      y <- Y[i]
      class.start.idx[y] <- i
      if (y == Y[n]) {
        break
      }
    }
  }
  return(class.start.idx)
}

rmat <- function(options) {
  p <- options[[1]] # number of dimensions
  d <- options[[2]] # this determines the number of columns in the projection matrix.
  method <- options[[3]] # defines the distribution of the random projection matrix
  #Create the random matrix, a sparse matrix of 1's, -1's, and 0's.
  if (method == "binary") {
    rho <- options[[4L]]
    nnzs <- round(p*d*rho)
    ind <- sort(sample.int((p*d), nnzs, replace = F))
    return(cbind(((ind - 1L) %% p) + 1L, floor((ind - 1L) / p) + 1L,
                 sample(c(1L, -1L), nnzs, replace = T)))
  } else if (method == "continuous") {
    rho <- options[[4L]]
    nnzs <- round(p*d*rho)
    ind <- sort(sample.int((p*d), nnzs, replace = F))
    return(cbind(((ind - 1L) %% p) + 1L, floor((ind - 1L) / p) + 1L,
                 runif(nnzs, min = -1, max = 1)))
  } else if (method == "rf") {
    return(cbind(sample.int(p, d, replace = F), 1:d, rep(1L, d)))
  } else if (method == "poisson") {
    lambda <- options[[4L]]
    go <- T
    while (go) {
      nnzPerCol <- rpois(d, lambda)
      go <- !any(nnzPerCol)
    }
    nnzPerCol[nnzPerCol > p] <- p
    nnz.cum <- cumsum(nnzPerCol)
    nz.rows <- integer(nnz.cum[d])
    nz.cols <- integer(nnz.cum[d])
    if (nnzPerCol[1L] != 0L) {
      nz.rows[1:nnzPerCol[1L]] <- sample.int(p, nnzPerCol[1L], replace = F)
      nz.cols[1:nnzPerCol[1L]] <- 1L
    }
    for (i in 2:d) {
      if (nnzPerCol[i] != 0L) {
        nz.rows[(nnz.cum[i - 1L] + 1L):nnz.cum[i]] <- sample.int(p, nnzPerCol[i], replace = F)
        nz.cols[(nnz.cum[i - 1L] + 1L):nnz.cum[i]] <- i
      }
    }
    # nz.rows <- c(unlist(sapply(nnzPerCol, function(x) sort(sample.int(p, x, replace = F)))))
    # nz.cols <- c(unlist(mapply(rep, 1:d, nnzPerCol)))
    return(cbind(nz.rows, nz.cols, sample(c(-1L,1L), nnz.cum[d], replace = T)))
  } else if (method == "frc") {
    nmix <- options[[4L]]
    nnz.cum <- seq.int(nmix, nmix*d, nmix)
    nz.rows <- integer(nnz.cum[d])
    nz.cols <- integer(nnz.cum[d])
    nz.rows[1:nmix] <- sample.int(p, nmix, replace = F)
    nz.cols[1:nmix] <- 1L
    for (i in 2:d) {
      nz.rows[(nnz.cum[i - 1L] + 1L):nnz.cum[i]] <- sample.int(p, nmix, replace = F)
      nz.cols[(nnz.cum[i - 1L] + 1L):nnz.cum[i]] <- i
    }
    return(cbind(nz.rows, nz.cols, runif(nnz.cum[d], -1, 1)))
  }
}

if (!require(compiler)) {
  randmat <- rmat
}

if (!exists("randmat")) {
  setCompilerOptions("optimize" = 3)
  randmat <- cmpfun(rmat)
}

rrot <- function(p) {
  #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  # Samples a p x p uniformly random rotation matrix via QR decomposition
  # of a matrix with elements sampled iid from a standard normal distribution
  #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  return(qr.Q(qr(matrix(rnorm(p^2), p, p))))
}
