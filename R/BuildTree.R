#' RerF Tree Generator
#'
#' Creates a single decision tree based on an input matrix and class vector.  This is the function used by rerf to generate trees.
#'
#' @param X an n by d numeric matrix (preferable) or data frame. The rows correspond to observations and columns correspond to features.
#' @param Y an n length vector of class labels.  Class labels must be integer or numeric and be within the range 1 to the number of classes.
#' @param FUN a function that creates the random projection matrix.
#' @param paramList parameters in a named list to be used by FUN. If left unchanged,
#' default values will be populated, see \code{\link[rerf]{defaults}} for details.
#' @param min.parent the minimum splittable node size.  A node size < min.parent will be a leaf node. (min.parent = 6)
#' @param max.depth the longest allowable distance from the root of a tree to a leaf node (i.e. the maximum allowed height for a tree).  If max.depth=0, the tree will be allowed to grow without bound.
#' @param bagging a non-zero value means a random sample of X will be used during tree creation.  If replacement = FALSE the bagging value determines the percentage of samples to leave out-of-bag.  If replacement = TRUE the non-zero bagging value is ignored.
#' @param replacement if TRUE then n samples are chosen, with replacement, from X.
#' @param stratify if TRUE then class sample proportions are maintained during the random sampling.  Ignored if replacement = FALSE.
#' @param class.ind a vector of lists.  Each list holds the indexes of its respective class (e.g. list 1 contains the index of each class 1 sample).
#' @param class.ct a cumulative sum of class counts.
#' @param store.oob if TRUE then the samples omitted during the creation of a tree are stored as part of the tree.  This is required to run OOBPredict().
#' @param store.impurity if TRUE then the reduction in Gini impurity is stored for every split. This is required to run FeatureImportance().
#' @param progress if true a pipe is printed after each tree is created.  This is useful for large datasets.
#' @param rotate if TRUE then the data matrix X is uniformly randomly rotated.
#'
#' @return Tree
#'
#' @examples
#' 
#' x <- iris[, -5]
#' y <- as.numeric(iris[, 5])
#' # BuildTree(x, y, RandMatBinary, p = 4, d = 4, rho = 0.25, prob = 0.5)
BuildTree <- function(X, Y, FUN, paramList, min.parent, max.depth, bagging, replacement,
                      stratify, class.ind, class.ct, store.oob, store.impurity, progress,
                      rotate) {
  # %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  # rfr builds a randomer classification forest structure made up of a list
  # of trees.  This forest is randomer because each node is rotated before
  # being split (as described by Tyler Tomita).  The tree is grown depth first.
  # The returned tree has a minimum of five vectors: a tree map, a probability
  # matrix, cutpoint vector, and two vectors needed to rotate data.
  #
  #  INPUT:
  #
  # X is an n-by-p matrix, where rows represent observations and columns
  # represent features
  #
  # Y is an n-by-1 array of integer class labels.
  #
  # FUN is the function used to create the projection matrix.  The matrix
  # returned by this function should be a p-by-u matrix where p is the
  # number of columns in the input matrix X and u is any integer > 0.
  # u can also vary from node to node.
  #
  # ... inputs to the user provided projection matrix
  # creation function -- FUN.
  #
  # min.parent is an integer specifying the minimum number of samples
  # a node must have in order for an attempt to split to be made.  Lower
  # values may lead to overtraining and increased training time.
  #
  # max.depth is the maximum depth that a tree can grow to.  If set to "0"
  # then there is no maximum depth.
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
  # store.impurity is a boolean that specifies whether to store the reduction in impurity of each
  # split.
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
  # %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


  FUN <- match.fun(FUN, descend = TRUE)

  # %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  # Predefine variables to prevent recreation during loops
  # %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  nClasses <- length(class.ct)
  ret <- list(MaxDeltaI = 0, BestVar = 0L, BestSplit = 0)
  currIN <- 0L # keep track of internal nodes for treemap
  currLN <- 0L # keep track of leaf nodes for treemap
  w <- nrow(X)
  p <- ncol(X)
  perBag <- (1 - bagging) * w
  Xnode <- double(w) # allocate space to store the current projection
  SortIdx <- integer(w)
  x <- double(w)
  y <- integer(w)

  # Calculate the Max Depth and the max number of possible nodes
  if (max.depth == 0L) {
    MaxNumNodes <- 2L * w
  } else {
    MaxNumNodes <- min(2L * w, 2L^(max.depth + 1L))
  }

  maxIN <- ceiling(MaxNumNodes / 2)
  treeMap <- integer(MaxNumNodes)
  ClassProb <- matrix(data = 0, nrow = maxIN, ncol = nClasses)
  CutPoint <- double(maxIN)
  # NdSize <- integer(MaxNumNodes)
  if (store.impurity) {
    delta.impurity <- double(maxIN)
  }
  NDepth <- integer(MaxNumNodes)
  Assigned2Node <- vector("list", MaxNumNodes)
  ind <- double(w)
  # Matrix A storage variables
  matAindex <- integer(maxIN)
  matAsize <- ceiling(w / 2)

  if (!identical(FUN, rerf::RandMatFRC) &&
    !identical(FUN, rerf::RandMatFRCN) &&
    !identical(FUN, rerf::RandMatContinuous) &&
    !identical(FUN, rerf::RandMatCustom)) {
    matAstore <- integer(matAsize)
  } else {
    matAstore <- double(matAsize)
  }
  matAindex[1L] <- 0L
  # %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  # %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  #                            Start tree creation
  # %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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
      go <- TRUE
      while (go) {
        # make sure each class is represented in proportion to classes in initial dataset
        if (stratify) {
          if (class.ct[1L] != 0L) {
            ind[1:class.ct[1L]] <- sample(class.ind[[1L]], class.ct[1L], replace = TRUE)
          }
          for (z in 2:nClasses) {
            if (class.ct[z - 1L] != class.ct[z]) {
              ind[(class.ct[z - 1L] + 1L):class.ct[z]] <- sample(class.ind[[z]], class.ct[z] - class.ct[z - 1L], replace = TRUE)
            }
          }
        } else {
          ind <- sample(1L:w, w, replace = TRUE)
        }
        go <- all(1L:w %in% ind)
      }
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
    NdSize <- length(Assigned2Node[[CurrentNode]]) # determine node size
    # determine class proportions in the node
    ClassCounts <- tabulate(Y[Assigned2Node[[CurrentNode]]], nClasses)
    ClProb <- ClassCounts / NdSize
    if (any(is.nan(ClProb))){
      return (BuildTree(X, Y, FUN, paramList, min.parent, max.depth, bagging, replacement, stratify, class.ind, class.ct, store.oob, store.impurity, progress, rotate))
    }
    # compute impurity for current node
    I <- sum(ClassCounts * (1 - ClProb))
    # check to see if node split should be attempted
    if (NdSize < min.parent ||
      I <= 0 ||
      NDepth[CurrentNode] == max.depth) {
      # store tree map data (negative value means this is a leaf node
      treeMap[CurrentNode] <- currLN <- currLN - 1L
      ClassProb[currLN * -1, ] <- ClProb
      NodeStack <- NodeStack[-1L] # pop node off stack
      Assigned2Node[[CurrentNode]] <- NA # remove saved indexes
      CurrentNode <- NodeStack[1L] # point to top of stack
      if (is.na(CurrentNode)) {
        break
      }
      next
    }

    # create projection matrix (sparseM) by calling the custom function FUN
    sparseM <- do.call(FUN, paramList)
    nnz <- nrow(sparseM) # the number of non zeroes in the sparse matrix
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

      # Project input into new space
      Xnode[1L:NdSize] <- X[Assigned2Node[[CurrentNode]], sparseM[lrows, 1L], drop = FALSE] %*% sparseM[lrows, 3L, drop = FALSE]

      # Sort the projection, Xnode, and rearrange Y accordingly
      SortIdx[1:NdSize] <- order(Xnode[1L:NdSize])
      x[1L:NdSize] <- Xnode[SortIdx[1L:NdSize]]
      y[1L:NdSize] <- Y[Assigned2Node[[CurrentNode]]][SortIdx[1:NdSize]]

      ##################################################################
      #                    Find Best Split
      ##################################################################
      # calculate deltaI for this rotation and return the best current deltaI
      # find split is an Rcpp call.
      ret[] <- findSplit(
        x = x[1:NdSize],
        y = y[1:NdSize],
        ndSize = NdSize,
        I = I,
        maxdI = ret$MaxDeltaI,
        bv = ret$BestVar,
        bs = ret$BestSplit,
        nzidx = nz.idx,
        cc = ClassCounts
      )

      nz.idx <- nz.idx + feature.nnz
    }

    # check to see if a valid split was found.
    if (ret$MaxDeltaI == 0) {
      # store tree map data (negative value means this is a leaf node
      treeMap[CurrentNode] <- currLN <- currLN - 1L
      ClassProb[currLN * -1L, ] <- ClProb
      NodeStack <- NodeStack[-1L] # pop current node off stack
      Assigned2Node[[CurrentNode]] <- NA # remove saved indexes
      CurrentNode <- NodeStack[1L] # point to current top of node stack
      if (is.na(CurrentNode)) {
        break
      }
      next
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
    Xnode[1:NdSize] <- X[Assigned2Node[[CurrentNode]], sparseM[lrows, 1L], drop = FALSE] %*% sparseM[lrows, 3L, drop = FALSE]

    # find which child node each sample will go to and move
    # them accordingly
    MoveLeft <- Xnode[1L:NdSize] <= ret$BestSplit

    # Move samples left or right based on split
    Assigned2Node[[NextUnusedNode]] <- Assigned2Node[[CurrentNode]][MoveLeft]
    Assigned2Node[[NextUnusedNode + 1L]] <- Assigned2Node[[CurrentNode]][!MoveLeft]

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
    if (matAindex[currIN] + currMatAlength > matAsize) { # grow the vector when needed.
      matAsize <- matAsize * 2L
      matAstore[matAsize] <- 0L
    }
    if (!identical(FUN, rerf::RandMatFRC) &&
      !identical(FUN, rerf::RandMatFRCN) &&
      !identical(FUN, rerf::RandMatContinuous) &&
      !identical(FUN, rerf::RandMatCustom)) {
      matAstore[(matAindex[currIN] + 1L):(matAindex[currIN] + currMatAlength)] <- as.integer(t(sparseM[lrows, c(1L, 3L)]))
    } else {
      matAstore[(matAindex[currIN] + 1L):(matAindex[currIN] + currMatAlength)] <- t(sparseM[lrows, c(1L, 3L)])
    }
    matAindex[currIN + 1] <- matAindex[currIN] + currMatAlength
    CutPoint[currIN] <- ret$BestSplit # store best cutpoint for this node
    if (store.impurity) {
      delta.impurity[currIN] <- ret$MaxDeltaI # store decrease in impurity for this node
    }

    Assigned2Node[[CurrentNode]] <- NA # remove saved indexes
    CurrentNode <- NodeStack[1L]
    if (is.na(CurrentNode)) {
      break
    }
  }

  currLN <- currLN * -1L
  # create tree structure and populate with mandatory elements
  tree <- list(
    "treeMap" = treeMap[1L:NextUnusedNode - 1L], "CutPoint" = CutPoint[1L:currIN], "ClassProb" = ClassProb[1L:currLN, , drop = FALSE],
    "matAstore" = matAstore[1L:matAindex[currIN + 1L]], "matAindex" = matAindex[1L:(currIN + 1L)], "ind" = NULL, "rotmat" = NULL,
    "rotdims" = NULL, "delta.impurity" = NULL
  )
  if (rotate) {
    tree$rotmat <- rotmat
    # is rotdims for > 1000 or < 1000? TODO
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
