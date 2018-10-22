#' RerF Tree Generator
#'
#' Creates a single decision tree based on an input matrix and class vector.  This is the function used by rerf to generate trees.
#'
#' @param X an n by d numeric matrix (preferable) or data frame. The rows correspond to observations and columns correspond to features.
#' @param Y an n length vector of class labels.  Class labels must be integer or numeric and be within the range 1 to the number of classes.
#' @param FUN a function that creates the random projection matrix.
#' @param paramList a named list of parameters to be used by FUN.  (paramList = list(p = ncol(X), d = round(ncol(X)^.5),1L, prob = 1/ncol(X)))
#' @param min.parent the minimum splittable node size.  A node size < min.parent will be a leaf node. (min.parent = 6)
#' @param max.depth the longest allowable distance from the root of a tree to a leaf node (i.e. the maximum allowed height for a tree).  If max.depth=0, the tree will be allowed to grow without bound.  (max.depth=0)
#' @param bagging a non-zero value means a random sample of X will be used during tree creation.  If replacement = FALSE the bagging value determines the percentage of samples to leave out-of-bag.  If replacement = TRUE the non-zero bagging value is ignored. (bagging=.2)
#' @param replacement if TRUE then n samples are chosen, with replacement, from X. (replacement=TRUE)
#' @param store.oob if TRUE then the samples omitted during the creation of a tree are stored as part of the tree.  This is required to run OOBPredict(). (store.oob=FALSE)
#' @param store.impurity if TRUE then the reduction in Gini impurity is stored for every split. This is required to run FeatureImportance() (store.impurity=FALSE)
#' @param progress if true a pipe is printed after each tree is created.  This is useful for large datasets. (progress=FALSE)
#' @param rotate if TRUE then the data matrix X is uniformly randomly rotated. (rotate=FALSE)
#'
#' @return Tree
#'
# TODO: add example

RegressionTree <-
  function(X,
           Y,
           FUN,
           paramList,
           min.parent,
           max.depth,
           bagging,
           replacement,
           store.oob,
           store.impurity,
           progress,
           rotate) {
    # TODO: Add description


    FUN <- match.fun(FUN, descend = TRUE)

    #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    # Predefine variables to prevent recreation during loops
    #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    ret <- list(MaxDeltaI = 0,
                BestVar = 0L,
                BestSplit = 0)
    currIN <- 0L # keep track of internal nodes for treemap
    currLN <- 0L # keep track of leaf nodes for treemap
    w <- nrow(X)
    p <- ncol(X)
    perBag <- (1 - bagging) * w
    Xnode <-
      double(w) # allocate space to store the current projection
    SortIdx <- integer(w)
    x <- double(w)
    y <- integer(w)

    # Calculate the Max Depth and the max number of possible nodes
    if (max.depth == 0L) {
      MaxNumNodes <- 2L * w
    } else{
      MaxNumNodes <- min(2L * w, 2L ^ (max.depth + 1L))
    }

    maxIN <- ceiling(MaxNumNodes / 2)
    treeMap <- integer(MaxNumNodes)
    Regressors <- matrix(data = 0, nrow = maxIN)
    CutPoint <- double(maxIN)
    # NdSize <- integer(MaxNumNodes)
    if (store.impurity) {
      delta.impurity <- double(maxIN)
    }
    NDepth <- integer(MaxNumNodes)
    Assigned2Node <- vector("list", MaxNumNodes)
    ind <- double(w)
    #Matrix A storage variables
    matAindex <- integer(maxIN)
    matAsize <- ceiling(w / 2)

    if (mat.options[[3L]] != "frc" &&
        mat.options[[3L]] != "continuous" &&
        mat.options[[3L]] != "frcn" &&
        mat.options[[3L]] != "custom") {
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
        rotmat <- RandRot(1000L)
        rotdims <- sample.int(p, 1000L)
        X[, rotdims] <- X[, rotdims] %*% rotmat
      } else {
        rotmat <- RandRot(p)
        X[] <- X %*% rotmat
      }
    }

    # intialize values for new tree before processing nodes
    NDepth[1L] <- 1L
    CurrentNode <- 1L
    NextUnusedNode <- 2L
    NodeStack <- 1L
    ind[] <- 0L
    # Determine bagging set
    # Assigned2Node is the set of row indices of X assigned to current node
    if (bagging != 0) {
      if (replacement) {
        ind <- sample(1L:w, w, replace = TRUE)
        Assigned2Node[[1L]] <- ind
      } else {
        ind[1:perBag] <- sample(1:w, perBag, replace = FALSE)
        Assigned2Node[[1L]] <- ind[1:perBag]
      }
    } else {
      Assigned2Node[[1L]] <- 1:w
    }

    # main loop over nodes.  This loop ends when the node stack is empty.
    while (CurrentNode < NextUnusedNode) {
      NdSize <- length(Assigned2Node[[CurrentNode]]) #determine node size
      # compute impurity for current node
      nodeMean <- mean(Y[Assigned2Node[[CurrentNode]]])

      I <- sum((nodeMean - Y[Assigned2Node[[CurrentNode]]]) ^ 2)

      # check to see if node split should be attempted
      if (NdSize < min.parent ||
          I <= 0 ||
          NDepth[CurrentNode] == max.depth) {
        # store tree map data (negative value means this is a leaf node
        treeMap[CurrentNode] <- currLN <- currLN - 1L
        Regressors[currLN * -1] <- nodeMean
        NodeStack <- NodeStack[-1L] # pop node off stack
        Assigned2Node[[CurrentNode]] <- NA #remove saved indexes
        CurrentNode <- NodeStack[1L] # point to top of stack
        if (is.na(CurrentNode)) {
          break
        }
        next
      }

      # create projection matrix (sparseM) by calling the custom function fun
      sparseM <- fun(mat.options)
      nnz <-
        nrow(sparseM) # the number of non zeroes in the sparse matrix
      # set initial values for the find best split computation for the current node.
      ret$MaxDeltaI <- 0
      nz.idx <- 1L

      # Check each projection to determine which splits the best.
      while (nz.idx <= nnz) {
        # Parse sparseM to the column of the projection matrix at this iteration
        feature.idx <- sparseM[nz.idx, 2L]
        feature.nnz <- 0L
        while (sparseM[nz.idx + feature.nnz, 2L] == feature.idx) {
          feature.nnz <- feature.nnz + 1L
          if (nz.idx + feature.nnz > nnz) {
            break
          }
        }
        # lrows are the elements in sparseM that will be used to rotate the data
        lrows <- nz.idx:(nz.idx + feature.nnz - 1L)

        #Project input into new space
        Xnode[1L:NdSize] <-
          X[Assigned2Node[[CurrentNode]], sparseM[lrows, 1L], drop = FALSE] %*% sparseM[lrows, 3L, drop =
                                                                                          FALSE]

        #Sort the projection, Xnode, and rearrange Y accordingly
        SortIdx[1:NdSize] <- order(Xnode[1L:NdSize])
        x[1L:NdSize] <- Xnode[SortIdx[1L:NdSize]]
        y[1L:NdSize] <-
          Y[Assigned2Node[[CurrentNode]]][SortIdx[1:NdSize]]

        # Find non-duplicate projection values
        duplicateSequence <- as.integer(duplicated(x[1L:NdSize]))
        splitPoints <- which(duplicateSequence == 0)


        ##################################################################
        #                    Find Best Split
        ##################################################################
        # calculate deltaI for this rotation and return the best current deltaI
        # find split is an Rcpp call.

        ret[] <- findSplit(
          x = x[1:NdSize],
          y = as.double(y[1:NdSize]),
          splitPoints = splitPoints,
          ndSize = NdSize,
          I = I,
          maxdI = ret$MaxDeltaI,
          bv = ret$BestVar,
          bs = ret$BestSplit,
          nzidx = nz.idx,
          cc = c(1),
          task = 1
        )

        nz.idx <- nz.idx + feature.nnz
      }

      # Recalculate the best projection and reproject the data
      feature.idx <- sparseM[ret$BestVar, 2L]
      feature.nnz <- 0L
      while (sparseM[ret$BestVar + feature.nnz, 2L] == feature.idx) {
        feature.nnz <- feature.nnz + 1L
        if (ret$BestVar + feature.nnz > nnz) {
          break
        }
      }
      lrows <- ret$BestVar:(ret$BestVar + feature.nnz - 1L)
      Xnode[1:NdSize] <-
        X[Assigned2Node[[CurrentNode]], sparseM[lrows, 1L], drop = FALSE] %*% sparseM[lrows, 3L, drop =
                                                                                        FALSE]

      # find which child node each sample will go to and move
      # them accordingly
      MoveLeft <- Xnode[1L:NdSize]  <= ret$BestSplit

      # check to see if a valid split was found.
      move_left_number = length(Assigned2Node[[CurrentNode]][MoveLeft])
      if (ret$MaxDeltaI == 0 |
          NdSize == move_left_number | 0 == move_left_number)   {
        # store tree map data (negative value means this is a leaf node
        treeMap[CurrentNode] <- currLN <- currLN - 1L
        Regressors[currLN * -1L] <- nodeMean
        NodeStack <- NodeStack[-1L] # pop current node off stack
        Assigned2Node[[CurrentNode]] <- NA #remove saved indexes
        CurrentNode <-
          NodeStack[1L] # point to current top of node stack
        if (is.na(CurrentNode)) {
          break
        }
        next
      }
      # Move samples left or right based on split
      Assigned2Node[[NextUnusedNode]] <-
        Assigned2Node[[CurrentNode]][MoveLeft]
      Assigned2Node[[NextUnusedNode + 1L]] <-
        Assigned2Node[[CurrentNode]][!MoveLeft]

      # store tree map data (positive value means this is an internal node)
      treeMap[CurrentNode] <- currIN <- currIN + 1L
      NDepth[NextUnusedNode] = NDepth[CurrentNode] + 1L
      NDepth[NextUnusedNode + 1L] = NDepth[CurrentNode] + 1L
      # Pop the current node off the node stack
      # this allows for a breadth first traversal
      NodeStack <- NodeStack[-1L]
      # Push two nodes onto the stack
      NodeStack <- c(NextUnusedNode, NextUnusedNode + 1L, NodeStack)
      NextUnusedNode <- NextUnusedNode + 2L
      # Store the projection matrix for the best split
      currMatAlength <- length(sparseM[lrows, c(1L, 3L)])
      if (matAindex[currIN] + currMatAlength > matAsize) {
        #grow the vector when needed.
        matAsize <- matAsize * 2L
        matAstore[matAsize] <- 0L
      }
      if (mat.options[[3L]] != "frc" &&
          mat.options[[3L]] != "continuous" &&
          mat.options[[3L]] != "frcn" &&
          mat.options[[3L]] != "custom") {
        matAstore[(matAindex[currIN] + 1L):(matAindex[currIN] + currMatAlength)] <-
          as.integer(t(sparseM[lrows, c(1L, 3L)]))
      } else {
        matAstore[(matAindex[currIN] + 1L):(matAindex[currIN] + currMatAlength)] <-
          t(sparseM[lrows, c(1L, 3L)])
      }
      matAindex[currIN + 1] <- matAindex[currIN] + currMatAlength
      CutPoint[currIN] <-
        ret$BestSplit # store best cutpoint for this node
      if (store.impurity) {
        delta.impurity[currIN] <-
          ret$MaxDeltaI # store decrease in impurity for this node
      }

      Assigned2Node[[CurrentNode]] <- NA #remove saved indexes
      CurrentNode <- NodeStack[1L]
      if (is.na(CurrentNode)) {
        break
      }
    }

    currLN <- currLN * -1L
    # create tree structure and populate with mandatory elements
    tree <-
      list(
        "treeMap" = treeMap[1L:NextUnusedNode - 1L],
        "CutPoint" = CutPoint[1L:currIN],
        "Regressors" = Regressors[1L:currLN],
        "matAstore" = matAstore[1L:matAindex[currIN + 1L]],
        "matAindex" = matAindex[1L:(currIN + 1L)],
        "ind" = NULL,
        "rotmat" = NULL,
        "rotdims" = NULL,
        "delta.impurity" = NULL
      )
    if (rotate) {
      tree$rotmat <- rotmat
      #is rotdims for > 1000 or < 1000? TODO
      if (p > 1000L) {
        tree$rotdims <- rotdims
      }
    }

    if (bagging != 0 && store.oob) {
      tree$ind <- which(!(1L:w %in% ind))
    }
    if (store.impurity) {
      tree$delta.impurity <- delta.impurity[1L:currIN]
    }

    if (progress) {
      print("|")
    }
    return(tree)
  }
