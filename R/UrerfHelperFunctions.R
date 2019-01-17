#' Find minimizing BIC Cut for Vector
#'
#' @param X a one dimensional vector
#' 
#' @return list containing minimizing cut point and corresponding BIC score.
#'

BICCutFast <- function(X) {
  minVal <- min(X)
  maxVal <- max(X)
  minErr <- Inf
  finalvartype <- 0
  otherBIC <- 0
  if (minVal == maxVal) {
    return(NULL)
  }
  sizeX <- length(X)
  ## sort after removing zeros
  X <- sort(X[which(X != 0)])
  ## Number of Non-Zeros
  sizeNNZ <- length(X)
  sizeZ <- sizeX - sizeNNZ

  sumLeft <- 0
  sumRight <- sum(X)
  errLeft <- 0
  errRight <- 0
  meanLeft <- 0
  meanRight <- 0
  errCurr <- 0
  cutPoint <- 0
  varType <- 1
  ## if any are zero
  if (sizeZ) {
    meanRight <- sumRight / sizeNNZ
    minErr <- sum((X - meanRight)^2)
    cutPoint <- X[1] / 2
  } else {
    minErr <- Inf
  }

  if (sizeNNZ - 1) {
    index <- 1 
    for (m in X[1:(sizeNNZ - 1)]) {
      leftsize <- sizeZ + index
      rightsize <- sizeNNZ - index
      sumLeft <- sumLeft + m
      sumRight <- sumRight - m
      meanLeft <- sumLeft / leftsize
      meanRight <- sumRight / rightsize
      N1 = leftsize
      N2 = rightsize
      errLeft <- sum((X[1:index] - meanLeft)^2) + sizeZ * (meanLeft^2)
      errRight <- sum((X[(index + 1):sizeNNZ] - meanRight)^2)
      sig1 = (1/N1) * errLeft
      sig2 = (1/N2) * errRight

      #when sigma=0, the log term becomes undefined 
      if (sig1 == 0 || sig2==0)
          next
      pi1 = N1/(N1+N2)
      pi2 = N2/(N1+N2)
      sig_comb = (1/(N1+N2))*(errLeft + errRight)

      sum_log_pi_k1 = -1*N1*log(N1/(N1+N2))
      sum_log_pi_k2 = -1*N2*log(N2/(N1+N2))

      sum_log_norm1 = (N1/2)*log(2*3.14*sig1)+N1/2
      sum_log_norm2 = (N2/2)*log(2*3.14*sig2)+N2/2

      #unisig means that the two clusters are constrained to have the same variance
      #bisig means that the two clusters can have separate variances

      sum_log_norm1_unisig = (N1/2)*log(2*3.14*sig_comb) + (N1+N2)/2
      sum_log_norm2_unisig = (N2/2)*log(2*3.14*sig_comb) 

      sum_log_terms_bisig = sum_log_norm1 + sum_log_norm2 + sum_log_pi_k1 + sum_log_pi_k2
      sum_log_terms_unisig = sum_log_norm1_unisig + sum_log_norm2_unisig + sum_log_pi_k1 + sum_log_pi_k2

      if(sum_log_terms_bisig < sum_log_terms_unisig)
          varType = 2
      else
          varType = 1

      bic_score_bisig = 2*sum_log_terms_bisig + log(N1+N2)*(varType + 3)
      bic_score_unisig = 2*sum_log_terms_unisig + log(N1+N2)*(varType + 3)

      errCurr <- min(bic_score_bisig,  bic_score_unisig)

      # Determine if this split is currently the best option
      # If current error is lowest, then save current cut point.
      
      if (errCurr < minErr) {
        cutPoint <- (X[index] + X[index + 1]) / 2
        minErr <- errCurr
      }
      index <- index + 1
    }
  }
  return(c(cutPoint, minErr))
}

BICCutMclust <- function(X){
  X_data = data.frame(X)
  num_elements = (length(unique(X)))
  if (num_elements <= 1 )
    return (NULL)

  BIC <- mclustBIC(X_data, G=2, warn=TRUE, verbose=FALSE )
  mod1 <- Mclust(data.frame(X), G=2, x=BIC, verbose=FALSE)

  if (!is.null(mod1) && !is.na(mod1)){
    #Group the elements according to the cluster they belong to.
    #Then sort the elements in each cluster.
    X_data["classi"] <- mod1["classification"]
    X_1 = data.frame(dplyr::filter(X_data, classi==1))
    X_1_sorted <- X_1[order(X_1$X),]
    X_2 = data.frame(dplyr::filter(X_data, classi==2))
    X_2_sorted <- X_2[order(X_2$X),]
    if(nrow(X_1) == 0)
      return (NULL)
    if(nrow(X_2) == 0)
      return (NULL)

    #Determine the cutpoint
    cutpt1 = tail(X_1_sorted["X"], n=1)["X"]
    cutpt2 = tail(X_2_sorted["X"], n=1)["X"]
    BIC_score = BIC[1][1]
    cutpt = min(cutpt1, cutpt2)

    return(c(cutpt, BIC_score))
  }
  return (NULL)
}


#' Find minimizing Two Means Cut for Vector
#'
#' @param X a one dimensional vector
#'
#' @return list containing minimizing cut point and corresponding sum of left and right variances.
#'
#'

TwoMeansCut <- function(X) {
  minVal <- min(X)
  maxVal <- max(X)
  if (minVal == maxVal) {
    return(NULL)
  }
  sizeX <- length(X)
  ## sort after removing zeros
  X <- sort(X[which(X != 0)])
  ## Number of Non-Zeros
  sizeNNZ <- length(X)
  sizeZ <- sizeX - sizeNNZ

  sumLeft <- 0
  sumRight <- sum(X)
  errLeft <- 0
  errRight <- 0
  meanLeft <- 0
  meanRight <- 0
  errCurr <- 0
  cutPoint <- NULL

  ## if any are zero
  if (sizeZ) {
    meanRight <- sumRight / sizeNNZ
    minErr <- sum((X - meanRight)^2)
    cutPoint <- X[1] / 2
  } else {
    minErr <- Inf
  }

  if (sizeNNZ - 1) {
    index <- 1
    for (m in X[1:(sizeNNZ - 1)]) {
      leftsize <- sizeZ + index
      rightsize <- sizeNNZ - index
      sumLeft <- sumLeft + m
      sumRight <- sumRight - m
      meanLeft <- sumLeft / leftsize
      meanRight <- sumRight / rightsize
      ## Error left accounts for the zeros that were removed earlier.
      errLeft <- sum((X[1:index] - meanLeft)^2) + sizeZ * (meanLeft^2)
      errRight <- sum((X[(index + 1):sizeNNZ] - meanRight)^2)

      errCurr <- errLeft + errRight
      # Determine if this split is currently the best option
      ## If current error is lowest, then save current cut point.
      if (errCurr < minErr) {
        cutPoint <- (X[index] + X[index + 1]) / 2
        minErr <- errCurr
      }
      index <- index + 1
    }
  }
  return(c(cutPoint, minErr))
}

#' Determine if given input can be processed by Urerf.
#'
#' @param X an Nxd matrix or Data frame of numeric values.
#'
#' @return stops function execution and outputs error if invalid input is detected.
#'
#'


checkInputMatrix <- function(X) {
  if (is.null(X)) {
    stop("the input is null.")
  }
  if (sum(is.na(X)) | sum(is.nan(X))) {
    stop("some values are na or nan.")
  }
  if (sum(colSums(X) == 0) != 0) {
    stop("some columns are all zero.")
  }
}


#' Creates Urerf Tree.
#'
#' @param X an Nxd matrix or Data frame of numeric values.
#' @param MinParent the minimum splittable node size (MinParent=1).
#' @param trees the number of trees to grow in a forest (trees=100).
#' @param MaxDepth the maximum depth allowed in a forest (MaxDepth=Inf).
#' @param bagging only used experimentally.  Determines the hold out size if replacement=FALSE (bagging=.2).
#' @param replacement method used to determine boot strap samples (replacement=TRUE).
#' @param FUN the function to create the rotation matrix used to determine mtry features.
#' @param options options provided to FUN.
#' @param Progress logical that determines whether to show tree creation status (Progress=TRUE).
#' @param LinearCombo logical that determines whether to use linear combination of features. (LinearCombo=TRUE).
#' @param splitCrit split based on twomeans(splitCrit="twomeans") or BIC test(splitCrit="BIC")
#'
#' @return tree
#'
#' @importFrom utils flush.console

GrowUnsupervisedForest <-
  function(X, MinParent = 1, trees = 100,
             MaxDepth = Inf, bagging = 0.2,
             replacement = TRUE, FUN = makeAB,
             options = list(p = ncol(X), d = ceiling(ncol(X)^0.5), sparsity = 1 / ncol(X)),
             Progress = TRUE, splitCrit = "twomeans", LinearCombo = TRUE) {
		if (LinearCombo) {
      FUN <- match.fun(FUN, descend = TRUE)
		} else {
      FUN <- match.fun(makeA, descend = TRUE)
		}
    ############# Start Growing Forest #################

    forest <- vector("list", trees)
    BV <- NA # vector in case of ties
    BS <- NA # vector in case of ties
    MaxDeltaI <- 0
    nBest <- 1L
    BestIdx <- 0L
    BestVar <- 0L
    BestSplitIdx <- 0L
    BestSplitValue <- 0
    w <- nrow(X)
    p <- ncol(X)
    perBag <- (1 - bagging) * w
    Xnode <- double(w) # allocate space to store the current projection
    SortIdx <- integer(w)
    if (object.size(X) > 1e+06) {
      OS <- TRUE
    } else {
      OS <- FALSE
    }

    # Calculate the Max Depth and the max number of possible nodes
    if (MaxDepth == Inf) {
      StopNode <- 2L * w # worst case scenario is 2*(w/(minparent/2))-1
      MaxNumNodes <- 2L * w # number of tree nodes for space reservation
    } else {
      if (MaxDepth == 0) {
        MaxDepth <- ceiling(log2(w))
      }
      StopNode <- 2L^(MaxDepth)
      MaxNumNodes <- 2L^(MaxDepth + 1L) # number of tree nodes for space reservation
    }

    CutPoint <- double(MaxNumNodes)
    Children <- matrix(data = 0L, nrow = MaxNumNodes, ncol = 2L)
    NDepth <- integer(MaxNumNodes)
    matA <- vector("list", MaxNumNodes)
    Assigned2Node <- vector("list", MaxNumNodes)
    Assigned2Leaf <- vector("list", MaxNumNodes)
    Assigned2Bag <- vector("list", MaxNumNodes)
    ind <- double(w)
    min_error <- Inf
    # %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    # %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    # Start tree creation
    # %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    for (treeX in 1:trees) {
      # intialize values for new tree before processing nodes
      CutPoint[] <- 0
      Children[] <- 0L
      NDepth[] <- 0L
      NDepth[1] <- 1L
      CurrentNode <- 1L
      NextUnusedNode <- 2L
      NodeStack <- 1L
      highestParent <- 1L
      Assigned2Leaf <- vector("list", MaxNumNodes)
      ind[] <- 0L
      # Determine bagging set Assigned2Node is the set of row indices of X assigned to
      # current node
      if (bagging != 0) {
        if (replacement) {
          ind <- sample(1:w, w, replace = TRUE)
          Assigned2Node[[1]] <- ind
        } else {
          ind[1:perBag] <- sample(1:w, perBag, replace = FALSE)
          Assigned2Node[[1]] <- ind[1:perBag]
        }
      } else {
        Assigned2Node[[1]] <- 1:w
      }
      Assigned2Bag[[1]] <- 1:w
      # main loop over nodes
      while (CurrentNode < NextUnusedNode && CurrentNode < StopNode) {
        # determine working samples for current node.
        NodeRows <- Assigned2Node[CurrentNode]
        Assigned2Node[[CurrentNode]] <- NA # remove saved indexes
        NdSize <- length(NodeRows[[1L]]) # determine node size

        sparseM <- do.call(FUN, options)

        if (NdSize < MinParent ||
          NDepth[CurrentNode] == MaxDepth ||
          NextUnusedNode + 1L >= StopNode ||
          NdSize == 1) {
          Assigned2Leaf[[CurrentNode]] <- Assigned2Bag[[CurrentNode]]
          # Assigned2Leaf[[CurrentNode]] <- NodeRows[[1L]]
          NodeStack <- NodeStack[-1L]
          CurrentNode <- NodeStack[1L]
          if (is.na(CurrentNode)) {
            break
          }
          next
        }
        min_error <- Inf
        cut_val <- 1
        BestVar <- 1

        # nBest <- 1L
        for (q in unique(sparseM[, 2])) {
          # Project input into new space
          lrows <- which(sparseM[, 2] == q)
          Xnode[1:NdSize] <- X[NodeRows[[1L]], sparseM[lrows, 1], drop = FALSE] %*%
            sparseM[lrows, 3, drop = FALSE]
          # Sort the projection, Xnode, and rearrange Y accordingly
          if (splitCrit == "twomeans")
            results <- TwoMeansCut(Xnode[1:NdSize])
          else if (splitCrit == "bicfast")
            results <- BICCutFast(Xnode[1:NdSize])
          else
            results <- BICCutMclust(Xnode[1:NdSize])
          if (is.null(results)) {
            next
          }

          if (results[2] < min_error) {
            cut_val <- results[1]
            min_error <- results[2]
            bestVar <- q
          }
        } # end loop through projections.

        if (min_error == Inf) {
          Assigned2Leaf[[CurrentNode]] <- Assigned2Bag[[CurrentNode]]
          # Assigned2Leaf[[CurrentNode]] <- NodeRows[[1L]]
          NodeStack <- NodeStack[-1L]
          CurrentNode <- NodeStack[1L]
          if (is.na(CurrentNode)) {
            break
          }
          next
        }

        # Recalculate the best projection
        lrows <- which(sparseM[, 2L] == bestVar)
        Xnode[1:NdSize] <-
          X[NodeRows[[1L]], sparseM[lrows, 1], drop = FALSE] %*%
          sparseM[lrows, 3, drop = FALSE]
        XnodeBag <-
          X[Assigned2Bag[[CurrentNode]], sparseM[lrows, 1], drop = FALSE] %*%
          sparseM[lrows, 3, drop = FALSE]


        # find which child node each sample will go to and move them accordingly changed
        # this from <= to < just in case best split split all values
        MoveLeft <- Xnode[1:NdSize] < cut_val
        numMove <- sum(MoveLeft)

        MoveBagLeft <- XnodeBag < cut_val

        if (is.null(numMove)) {
          print("numMove is null")
          flush.console()
        }
        if (is.na(numMove)) {
          print("numMove is na")
          flush.console()
        }
        # Check to see if a split occured, or if all elements being moved one direction.
        if (numMove != 0L && numMove != NdSize) {
          # Move samples left or right based on split
          Assigned2Node[[NextUnusedNode]] <- NodeRows[[1L]][MoveLeft]
          Assigned2Node[[NextUnusedNode + 1L]] <- NodeRows[[1L]][!MoveLeft]

          Assigned2Bag[[NextUnusedNode]] <- Assigned2Bag[[CurrentNode]][MoveBagLeft]
          Assigned2Bag[[NextUnusedNode + 1L]] <- Assigned2Bag[[CurrentNode]][!MoveBagLeft]


          # highest Parent keeps track of the highest needed matrix and cutpoint this
          # reduces what is stored in the forest structure
          if (CurrentNode > highestParent) {
            highestParent <- CurrentNode
          }
          # Determine children nodes and their attributes
          Children[CurrentNode, 1L] <- NextUnusedNode
          Children[CurrentNode, 2L] <- NextUnusedNode + 1L
          NDepth[NextUnusedNode] <- NDepth[CurrentNode] + 1L
          NDepth[NextUnusedNode + 1L] <- NDepth[CurrentNode] + 1L
          # Pop the current node off the node stack this allows for a breadth first
          # traversal
          Assigned2Leaf[[CurrentNode]] <- Assigned2Bag[[CurrentNode]]
          NodeStack <- NodeStack[-1L]
          NodeStack <- c(NextUnusedNode, NextUnusedNode + 1L, NodeStack)
          NextUnusedNode <- NextUnusedNode + 2L
          # Store the projection matrix for the best split
          matA[[CurrentNode]] <- as.integer(base::t(sparseM[which(sparseM
          [
            ,
            2
          ] == bestVar), c(1, 3)]))
          CutPoint[CurrentNode] <- cut_val
        } else {
          # There wasn't a good split so ignore this node and move to the next

          Assigned2Leaf[[CurrentNode]] <- Assigned2Bag[[CurrentNode]]
          NodeStack <- NodeStack[-1L]
        }
        # Store ClassProbs for this node.  Only really useful for leaf nodes, but could
        # be used instead of recalculating at each node which is how it is currently.

        Assigned2Bag[[CurrentNode]] <- NA # remove saved indexes
        CurrentNode <- NodeStack[1L]
        if (is.na(CurrentNode)) {
          break
        }
      }
      # If input is large then garbage collect prior to adding onto the forest
      # structure.
      if (OS) {
        gc()
      }
      # save current tree structure to the forest
      forest[[treeX]] <- list(CutPoint = CutPoint[1:highestParent], Children = Children[1L:(NextUnusedNode -
        1L), , drop = FALSE], matA = matA[1L:highestParent], ALeaf = Assigned2Leaf[1L:(NextUnusedNode -
        1L)], TrainSize = nrow(X))

      if (Progress) {
        cat("|")
        flush.console()
      }
    }
    return(forest)
  }
