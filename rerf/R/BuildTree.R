#' RerF Tree Generator
#'
#' Creates a single decision tree based on an input matrix and class vector.  This is the function used by rerf to generate trees.
#'
#' @param X an n by d numeric matrix (preferable) or data frame. The rows correspond to observations and columns correspond to features.
#' @param Y an n length vector of class labels.  Class labels must be integer or numeric and be within the range 1 to the number of classes.
#' @param min.parent the minimum splittable node size.  A node size < min.parent will be a leaf node. (min.parent = 6)
#' @param max.depth the longest allowable distance from the root of a tree to a leaf node (i.e. the maximum allowed height for a tree).  If max.depth=0, the tree will be allowed to grow without bound.  (max.depth=0)  
#' @param bagging a non-zero value means a random sample of X will be used during tree creation.  If replacement = FALSE the bagging value determines the percentage of samples to leave out-of-bag.  If replacement = TRUE the non-zero bagging value is ignored. (bagging=.2) 
#' @param replacement if TRUE then n samples are chosen, with replacement, from X. (replacement=TRUE)
#' @param stratify if TRUE then class sample proportions are maintained during the random sampling.  Ignored if replacement = FALSE. (stratify = FALSE).
#' @param class.ind a vector of lists.  Each list holds the indexes of its respective class (e.g. list 1 contains the index of each class 1 sample).
#' @param class.ct a cumulative sum of class counts.  
#' @param fun a function that creates the random projection matrix. (fun=makeA) 
#' @param options a list of parameters to be used by fun. (options=c(ncol(X), round(ncol(X)^.5),1L, 1/ncol(X)))
#' @param store.oob if TRUE then the samples omitted during the creation of a tree are stored as part of the tree.  This is required to run OOBPredict(). (store.oob=FALSE)
#' @param store.ns if TRUE then the number of training observations at each node is stored. This is required to run FeatureImportance() (store.ns=FALSE) (store.ns=FALSE)
#' @param progress if true a pipe is printed after each tree is created.  This is useful for large datasets. (progress=FALSE)
#' @param rotate if TRUE then the data matrix X is uniformly randomly rotated. (rotate=FALSE)
#'
#' @return Tree
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#' 

BuildTree <-
function(X, Y, min.parent, max.depth, bagging, replacement, stratify, class.ind, class.ct, fun, options, store.oob, store.ns, progress, rotate){
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
  # min.parent is an integer specifying the minimum number of observations
  # a node must have in order for an attempt to split to be made.  Lower
  # values may lead to overtraining and increased training time.
  #
  # trees is the number of trees that will be in the forest.
  #
  # max.depth is the maximum depth that a tree can grow to.  If set to "inf"
  # then there is no maximum depth.  If set to 0 then a maximum depth is
  # calculated based on the number of classes and number of samples provided.
  #
  # bagging is the percentage of training data to withhold during each
  # training iteration.  If set to 0 then the entire training set is used
  # during every iteration.  The withheld portion of the training data
  # is used to calculate OOB error for the tree.
  #
  # class.ct is the number of different classes in Y.  It is calculated 
  # in the calling function to prevent recalculation by each forked function 
  # when in parallel.
  #
  # fun is the function used to create the projection matrix.  The matrix
  # returned by this function should be a p-by-u matrix where p is the
  # number of columns in the input matrix X and u is any integer > 0.
  # u can also vary from node to node.
  #
  # options is a list of inputs to the user provided projection matrix
  # creation function -- fun.
  #
  # rotate is a boolean specifying whether or not to randomly rotate the
  # for each tree. If TRUE, then a different random rotation will be applied
  # to each bagged subsample prior to building each tree. If the number of
  # dimensions is greater than 1000, then a random subset of 1000 of the
  # dimensions will be rotated and the others will be left alone
  #
  # store.oob is a boolean that determines whether or not OOB error is calculated.
  # If bagging equals zero then store.oob is ignored.  If bagging does not equal 
  # zero and store.oob is TRUE then OOB is calculated and printed to the screen.
  # 
  # store.ns is a boolean that specifies whether to store the node size of each
  # node.
  #
  # progress is a boolean.  When true a progress marker is printed to the 
  # screen every time a tree is grown.  This is useful for large input.
  #
  # OUTPUT:
  #
  # A forest construct made up of trees.  This forest can be used to make 
  # predictions on new inputs.  When store.oob=TRUE then the output is a list
  # containing $forest and $OOBmat.  $forest is the forest structure and
  # OOBmat is the OOB error for each tree.
  #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  
  #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  # Predefine variables to prevent recreation during loops
  #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  nClasses <- length(class.ct)
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
  
  # Calculate the Max Depth and the max number of possible nodes
  if(max.depth == "inf"){
    MaxNumNodes <- 2L*w # number of tree nodes for space reservation
  }else{
    if(max.depth==0){
      max.depth <- ceiling((log2(w)+log2(nClasses))/2)
    }
    MaxNumNodes <- 2L^(max.depth+1L)  # number of tree nodes for space reservation
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
  if (options[[3]] != "frc" && options[[3]] != "continuous" && options[[3]] != "frcn") {
    matAstore <- integer(matAsize)
  } else {
    matAstore <- double(matAsize)
  }
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
      X[] <- X%*%rotmat
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
          if (class.ct[1L] != 0L) {
            ind[1:class.ct[1L]]<-sample(class.ind[[1L]], class.ct[1L], replace=TRUE)
          }
          for (z in 2:nClasses) {
            if (class.ct[z - 1L] != class.ct[z]) {
              ind[(class.ct[z - 1L] + 1L):class.ct[z]]<- sample(class.ind[[z]], class.ct[z] - class.ct[z - 1L], replace=TRUE)
            }
          }
        } else {
          ind<-sample(1:w, w, replace=TRUE)
        }
        go <- all(1:w %in% ind)
      }
      Assigned2Node[[1L]] <- ind
    } else {
      ind[1:perBag] <- sample(1:w, perBag, replace = FALSE)
      Assigned2Node[[1L]] <- ind[1:perBag]        
    }
  } else {
    Assigned2Node[[1L]] <- 1:w        
  }
  
  # main loop over nodes
  while (CurrentNode < NextUnusedNode) {
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
    if (NdSize[CurrentNode] < min.parent || I <= 0 || NDepth[CurrentNode]==max.depth){
      treeMap[CurrentNode] <- currLN <- currLN - 1L
      ClassProb[currLN*-1,] <- ClProb
      NodeStack <- NodeStack[-1L]
      CurrentNode <- NodeStack[1L]
      if(is.na(CurrentNode)){
        break
      }
      next 
    }
    
    # create projection matrix (sparseM) by calling the custom function fun
    sparseM <- fun(options)
    nnz <- nrow(sparseM)
    # Check each projection to determine which splits the best.
    ret$MaxDeltaI <- 0
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
    if (options[[3]] != "frc" && options[[3]] != "continuous" && options[[3]] != "frcn") {
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
      if (store.ns) {
        if(bagging!=0 && store.oob){
          tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                       "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "ind"=which(!(1:w %in% ind)),
                       "NdSize" = NdSize[1L:(NextUnusedNode-1L)], "rotmat" = rotmat, "rotdims" = rotdims)
        }else{
          tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                       "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "NdSize" = NdSize[1L:(NextUnusedNode-1L)],
                       "rotmat" = rotmat, "rotdims" = rotdims)
        }
      } else {
        if(bagging!=0 && store.oob){
          tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                       "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "ind"=which(!(1:w %in% ind)),
                       "rotmat" = rotmat, "rotdims" = rotdims)
        }else{
          tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                       "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "rotmat" = rotmat, "rotdims" = rotdims)
        }
      }
    } else {
      if (store.ns) {
        if(bagging!=0 && store.oob){
          tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                       "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "ind"=which(!(1:w %in% ind)),
                       "NdSize" = NdSize[1L:(NextUnusedNode-1L)], "rotmat" = rotmat)
        }else{
          tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                       "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "NdSize" = NdSize[1L:(NextUnusedNode-1L)],
                       "rotmat" = rotmat)
        }
      } else {
        if(bagging!=0 && store.oob){
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
    if (store.ns) {
      if(bagging!=0 && store.oob){
        tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                     "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "ind"=which(!(1:w %in% ind)), "NdSize" = NdSize[1L:(NextUnusedNode-1L)])
      }else{
        tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                     "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "NdSize" = NdSize[1L:(NextUnusedNode-1L)])
      }
    } else {
      if(bagging!=0 && store.oob){
        tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                     "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)], "ind"=which(!(1:w %in% ind)))
      }else{
        tree <- list("treeMap"=treeMap[1:NextUnusedNode-1L], "CutPoint"=CutPoint[1:currIN],"ClassProb"=ClassProb[1L:currLN,,drop=FALSE],"Children"=Children[1L:currIN],
                     "matAstore"=matAstore[1:matAindex[currIN+1]], "matAindex"=matAindex[1L:(currIN+1)])
      }
    }
  }
  
  if(progress){
    cat("|")
  }
  return(tree)
}
