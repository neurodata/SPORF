rfr <- function(X, Y, mtry=0, MinParent=6, trees=100, MaxDepth=0, bagging = .20){
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
    # mtry is an integer less than p used to specify the number of non-zero
    # values in the rotation matrix.  The rotation matrix is a p-by-mtry matrix
    # with up to mtry+mtry non-zero values.
    #
    # MinParent is an integer specifying the minimum number of observations
    # a node must have in order for an attempt to split to be made.
    #
    # Trees is the number of trees that will be in the forest.
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
    # OUTPUT:
    #
    # A forest construct made up of trees.  This forest can be used to make 
    # predictions on new inputs.
    #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    ########################################################
    ####Start main function
    ########################################################


    Ymat <- as.matrix(Y)
    X <- as.matrix(X)
    Yunique <- as.numeric(levels(as.factor(Ymat)))    # unique class labels
    nClasses <- length(Yunique) # number of classes

    err<-0 # used for OOB error
    forest <- vector("list",trees)

    w <- nrow(X)
    p <- ncol(X)

    if(mtry==0){
        mtry <- ceiling(p^.5)
    }

    # Control tree depth
    if(MaxDepth == "inf"){
        StopNode <- 2*w-1
        MaxNumNodes <- 2*w-1   # number of tree nodes for space reservation

    }else{
        if(MaxDepth==0){
            MaxDepth <- ceiling((log2(w)+log2(nClasses))/2)
        }
        StopNode <- 2^(MaxDepth)
        MaxNumNodes <- 2^(MaxDepth+1)  # number of tree nodes for space reservation
    }

    ClassProb <- matrix(data = 0, nrow = MaxNumNodes, ncol = nClasses)
    CutVar <- matrix(data = 0, nrow = MaxNumNodes, ncol = 1)
    CutPoint <- matrix(data = 0, nrow = MaxNumNodes,ncol = 1)
    Children <- matrix(data = 0, nrow = MaxNumNodes,ncol = 2)
    NodeSize <- matrix(data = 0, nrow = MaxNumNodes,ncol = 1)
    matA <- vector("list",MaxNumNodes)

    for(i in 1:trees){
        # prepare return values for new tree
        ClassProb[] <- 0 
        CutVar[] <- 0     
        CutPoint[] <- 0
        Children[] <- 0
        NodeSize[]<- 0


        # set bagging and cross 
        ind <- sample(c(1,2), w, replace = TRUE, prob = c(1-bagging, bagging))
        Xtrain <- X[ind ==1,]
        Xtest <- X[ind ==2,]
        Ytrain <- Ymat[ind ==1]
        Ytest <- Ymat[ind ==2]

        n <- nrow(Xtrain)
        #initialize variable for new tree
        Assigned2Node <- list(1:n)# Assigned2Node is the set of row indices of X assigned to current node
        NodeSize[1] <- n
        CurrentNode <- 1
        NextUnusedNode <- 2

        # main loop over nodes
        while (CurrentNode <= NextUnusedNode && CurrentNode < StopNode){
            # prepare loop for current node
            NodeRows <- Assigned2Node[CurrentNode]
            NdSize <- NodeSize[CurrentNode]
            Xnode <- Xtrain[NodeRows[[1]],]
            Ynode <- Ytrain[NodeRows[[1]]]

            # determine number of samples in current node then
            # determine their percentages in the node
            ClassCounts <- matrix(data=0, nrow=1,ncol=nClasses)
            for (c in 1:nClasses){
                Cl = Yunique[c]
                ClassCounts[c] = sum(Ynode==Cl)
            }
            ClProb <- ClassCounts/NdSize
            # compute impurity for current node
            I <- NdSize*sum(ClProb*(1 - ClProb))

            # if node is impure and large enough then attempt to find good split
            if (NdSize >= MinParent && I > 0){
                ###########################################################
                # this is the randomer part of random forest. The sparseM 
                # matrix is the rotation matrix.  The creation of this
                # matrix can be changed, but the nrow of sparseM should
                # remain p.  The ncol of the sparseM matrix is currently
                # set to mtry but this can actually be any integer > 1;
                # can even greater than p.
                ###########################################################
                #Create the A matrix, a sparse matrix of 1's, -1's, and 0's.
                sparseM <- matrix(0,nrow=p,ncol=mtry)
                sparseM[sample(1:(p*mtry),mtry,replace=F)] <- 1
                sparseM[sample(1:(p*mtry),mtry,replace=F)] <- -1
                #The below gets rid of zero columns in sparseM.
                sparseM <- sparseM[,!apply(sparseM==0,2,all)]
                # Ensure sparseM is still a matrix because if it is reduced to one column then it gets converted to a list.
                if (class(sparseM)=="matrix"){
                    NZcols <- ncol(sparseM)
                    # rotate node
                    Xnode <- Xnode%*%sparseM
                }else{
                    NZcols <- 1 
                    # rotate node
                    Xnode <- as.matrix(Xnode%*%as.matrix(sparseM))
                }
                # store sparseM in the forest structure to be used in the predict function.
                matA[[CurrentNode]] <- sparseM
                ###########################################################

                y <- matrix(rep(Ynode,NZcols), ncol=NZcols)
                #Initialize a matrix to store the correct number of columns.
                if (NdSize > 1){
                    SortIdx <- apply(Xnode,2,order)
                    for (j in 1:NZcols){
                        y[,j] <- y [SortIdx[,j],j]
                    }
                }
                ##################################################################
                ##### Find Best Split
                ##################################################################
                q <- nrow(y)
                d <- NZcols

                BV <- matrix(data=0, nrow=q*d, ncol= 1)
                BS <- matrix(data=0, nrow=q*d, ncol =1)
                ClassCountsLeft <- vector("integer", nClasses)
                ClassCountsRight <- vector("integer", nClasses)

                MaxDeltaI <- 0
                nBest <- 1
                nClasses <- length(ClassCounts)
                for (j in 1:d){
                    consecutive <- 0
                    ClassCountsLeft[] <- 0
                    ClassCountsRight[] <- ClassCounts
                    yl <- y[1,j]
                    for (m in 1:(q-1)){
                        yr <- y[m+1,j]
                        consecutive <- consecutive +1
                        if (yl == yr ){
                            next
                        }
                        ClassCountsLeft[yl] <- ClassCountsLeft[yl] + consecutive
                        ClassCountsRight[yl] <- ClassCountsRight[yl] - consecutive
                        consecutive <- 0
                        yl <- yr

                        ClassProbLeft <- ClassCountsLeft/m
                        ClassProbRight <- ClassCountsRight/(q-m)
                        DeltaI <- I - sum(ClassCountsLeft*(1 - ClassProbLeft)) - sum(ClassCountsRight*(1 - ClassProbRight))

                        if (DeltaI > MaxDeltaI){
                            MaxDeltaI <- DeltaI
                            nBest <- 1
                            BV[nBest] <- j
                            BS[nBest] <- m
                        }else {
                            if (DeltaI == MaxDeltaI){
                                nBest <- nBest + 1
                                BV[nBest] <- j
                                BS[nBest] <- m
                            }
                        }
                    }
                }
                DeltaI <- MaxDeltaI
                # Break ties at random
                if (nBest > 1){
                    BestIdx <- ceiling(runif(1,0,nBest))
                    BestVar <- BV[BestIdx]
                    BestSplitIdx <- BS[BestIdx]
                }else{
                    BestVar <- BV[1]
                    BestSplitIdx <- BS[1]
                }
                # Save best split info in CurrentBest
                CurrentBest <- list("BestVar" = BestVar, "SplitValueIdx" = BestSplitIdx)
            }else{
                CurrentBest <- list("BestVar" = 0, "SplitValueIdx" = 0)
            }

            # if good split was found, move data to left and right
            if (CurrentBest$BestVar != 0){
                BestSplitValue <- mean(Xnode[SortIdx[CurrentBest$SplitValueIdx:(CurrentBest$SplitValueIdx+1),CurrentBest$BestVar],CurrentBest$BestVar])
                MoveLeft <- Xnode[,CurrentBest$BestVar] <= BestSplitValue
                Assigned2Node <- c(Assigned2Node, list(NodeRows[[1]][MoveLeft]))
                Assigned2Node <- c(Assigned2Node, list(NodeRows[[1]][!MoveLeft]))
                Children[CurrentNode,1] <- NextUnusedNode
                Children[CurrentNode,2] <- NextUnusedNode+1

                NodeSize[NextUnusedNode,1] <- length(Assigned2Node[[NextUnusedNode]])
                NodeSize[NextUnusedNode+1,1] <- length(Assigned2Node[[NextUnusedNode+1]])

                NextUnusedNode <- NextUnusedNode + 2

                CutVar[CurrentNode,1] <- CurrentBest$BestVar
                CutPoint[CurrentNode,1] <- BestSplitValue
            }else{
                CutVar[CurrentNode,1] <- 0
                CutPoint[CurrentNode,1] <- 0
            }
            ClassProb[CurrentNode,] <- ClProb
            CurrentNode <- CurrentNode + 1
        }


        forest[[i]] <- list("CutVar"=CutVar[1:NextUnusedNode],"CutPoint"=CutPoint[1:NextUnusedNode],"ClassProb"=ClassProb[1:NextUnusedNode,],"Children"=Children[1:NextUnusedNode,], "matA"=matA[1:NextUnusedNode])
        if(bagging!=0){
            err <- err + error_rate(Xtest, Ytest, forest[i])
        }
    }
    if(bagging!=0){
        cat("\nOut-Of-Bag Error Rate -- ", err/trees)
    }
    return(forest)
}

#########################################################################
#######Calculate Error Rate
########################################################################
error_rate <- function(X,Y,Forest){
    X <- as.matrix(X)
    n <- nrow(X)
    numWrong <- 0
    forestSize <- length(Forest)
    z <- integer()
    for(i in 1:n){
        Z <- as.matrix(X[i,])
        classProb <- 0

        for(j in 1:forestSize){
            Tree <- Forest[[j]]
            currentNode <- 1
            while(Tree$Children[currentNode]!=0){
                rotX <- t(Z)%*%Tree$matA[[currentNode]]
                if(rotX[Tree$CutVar[currentNode]]<=Tree$CutPoint[currentNode]){
                    currentNode <- Tree$Children[currentNode,1]
                }else{
                    currentNode <- Tree$Children[currentNode,2]
                }
            }
            classProb <- classProb + Tree$ClassProb[currentNode,]
        }
        z <- c(z,order(classProb,decreasing=T)[1])
    }
    # compare predicted to actual then divide number wrong by total tested
    return(sum(z!=Y)/nrow(X))
}


#########################################################################
#######Make Predictions
########################################################################
predict <- function(X,Forest){
    X <- as.matrix(X)
    n <- nrow(X)
    numWrong <- 0
    forestSize <- length(Forest)
    z <- integer()
    for(i in 1:n){
        Z <- as.matrix(X[i,])
        classProb <- 0
        for(j in 1:forestSize){
            Tree <- Forest[[j]]
            currentNode <- 1
            while(Tree$Children[currentNode]!=0){
                rotX <- t(Z)%*%Tree$matA[[currentNode]]
                if(rotX[Tree$CutVar[currentNode]]<=Tree$CutPoint[currentNode]){
                    currentNode <- Tree$Children[currentNode,1]
                }else{
                    currentNode <- Tree$Children[currentNode,2]
                }
            }
            classProb <- classProb + Tree$ClassProb[currentNode,]
        }
        z <- c(z,order(classProb,decreasing=T)[1])
    }
    return(z)
}
