#runrfr <- function(X, Y, MinParent=6, trees=100, MaxDepth=0, bagging = .2, FUN=makeA, options=ncol(X)){
runrfr <- function(X, Y, MinParent, trees, MaxDepth, bagging, FUN, options){
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
    # FUN is the function used to create the projection matrix.  The matrix
    # returned by this function should be a p-by-u matrix where p is the
    # number of columns in the input matrix X and u is any integer > 0.
    # u can also vary from node to node.
    #
    # options is a list of inputs to the user provided projection matrix
    # creation function -- FUN.
    #
    # OUTPUT:
    #
    # A forest construct made up of trees.  This forest can be used to make 
    # predictions on new inputs.
    #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    #                            Start main function
    #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    Y <- as.matrix(Y)
    X <- as.matrix(X)
    Yunique <- as.numeric(levels(as.factor(Y)))    # unique class labels
    nClasses <- length(Yunique) # number of classes

    err<-0 # used for OOB error
    forest <- vector("list",trees)

    # Class Counts are used in the Find Best Split Function.
    ClassCountsLeft <- matrix(nrow=nClasses, ncol=1)
    ClassCountsRight <- matrix(nrow=nClasses, ncol=1)

    w <- nrow(X)
    p <- ncol(X)

    # Control tree depth
    if(MaxDepth == "inf"){
        StopNode <- 4*w/MinParent-1 #worst case scenario is 2*(w/(minparent/2))-1
        MaxNumNodes <- 4*w/MinParent-1 # number of tree nodes for space reservation
    }else{
        if(MaxDepth==0){
            MaxDepth <- ceiling((log2(w)+log2(nClasses))/2)
        }
        StopNode <- 2^(MaxDepth)
        MaxNumNodes <- 2^(MaxDepth+1)  # number of tree nodes for space reservation
    }

    ClassProb <- matrix(data = 0, nrow = MaxNumNodes, ncol = nClasses)
    #CutVar <- matrix(data = 0, nrow = MaxNumNodes, ncol = 1)
    CutPoint <- matrix(data = 0, nrow = MaxNumNodes,ncol = 1)
    Children <- matrix(data = 0, nrow = MaxNumNodes,ncol = 2)
    NDepth <- matrix(data = 0, nrow = MaxNumNodes, ncol =1)
    #NodeSize <- matrix(data = 0, nrow = MaxNumNodes,ncol = 1)
    matA <- matrix(data = 0, nrow = p, ncol = MaxNumNodes) 

    for(treeX in 1:trees){
        # prepare return values for new tree
        ClassProb[] <- 0
        #CutVar[] <- 0
        CutPoint[] <- 0
        Children[] <- 0
        NDepth[]<- 0
        #NodeSize[]<- 0
        matA[] <- 0
        NDepth[1]<-1
        # set bagging and cross
        ind <- sample(c(1,2), w, replace = TRUE, prob = c(1-bagging, bagging))
        Xtrain <- X[ind == 1,]
        Xtest <- X[ind == 2,]
        Ytrain <- Y[ind == 1]
        Ytest <- Y[ind == 2]

        n <- nrow(Xtrain)
        #initialize variable for new tree
        Assigned2Node <- list(1:n)# Assigned2Node is the set of row indices of X assigned to current node
        #NodeSize[1] <- n
        CurrentNode <- 1
        NextUnusedNode <- 2
        NodeStack <- 1
        highestParent <- 1

        # main loop over nodes
        while (CurrentNode < NextUnusedNode && CurrentNode < StopNode){
            # prepare loop for current node
            #cat(length(Assigned2Node[CurrentNode][[1]]), " ", CurrentNode,"\n")
            NodeRows <- Assigned2Node[CurrentNode]
            Assigned2Node[CurrentNode]<-NA
            NdSize <- length(NodeRows[[1]])#NodeSize[CurrentNode]
            Xnode <- Xtrain[NodeRows[[1]],]
            Ynode <- Ytrain[NodeRows[[1]]]
            #NdSize <- nrow(Xnode)
            #cat(NdSize, "--", is.null(nrow(Xnode)), "\n")

            # determine number of samples in current node then
            # determine their percentages in the node
            ClassCounts <- tabulate(Ynode, nClasses)

            ClProb <- ClassCounts/NdSize
            # compute impurity for current node
            I <- NdSize*sum(ClProb*(1 - ClProb))

            # if node is impure and large enough then attempt to find good split
            if (NdSize < MinParent || I <= 0 || NDepth[CurrentNode]==MaxDepth || NextUnusedNode+1 >= StopNode){
                ClassProb[CurrentNode,] <- ClProb
                NodeStack <- NodeStack[-1]
                CurrentNode <- NodeStack[1]
                if(is.na(CurrentNode)){
                    break
                }
                #NextUnusedNode <- NextUnsedNode + 2
                next 
            }
            # create projection matrix (sparseM) by calling the custom function FUN
            sparseM <- FUN(options)
            NZcols <- ncol(sparseM)
            # rotate the input matrix.  as.matrix is necessary when sparseM only has 1 column
            Xnode <- Xnode%*%sparseM

            y <- matrix(rep(Ynode,NZcols), ncol=NZcols)
            #Initialize a matrix to store the correct number of columns.
            SortIdx <- apply(Xnode,2,order)
            for (j in 1:NZcols){
                y[,j] <- y [SortIdx[,j],j]
            }
            ##################################################################
            #                    Find Best Split
            # 
            # Including Find Best Split in the main function was faster when
            # using the compiler function in R.
            ##################################################################
            BV <- NA # vector in case of ties
            BS <- NA # vector in case of ties

            MaxDeltaI <- 0
            nBest <- 1
            for (j in 1:NZcols){
                consecutive <- 0
                # initialize ClassCounts for loop
                ClassCountsLeft[] <- 0
                ClassCountsRight[] <- ClassCounts
                yl <- y[1,j]
                for (m in 1:(NdSize-1)){
                    yr <- y[m+1,j]
                    consecutive <- consecutive +1
                    # only calculate Class Probs if current class is different
                    # from the previous class
                    if (yl == yr ){
                        next
                    }
                    ClassCountsLeft[yl] <-ClassCountsLeft[yl] + consecutive 
                    ClassCountsRight[yl] <- ClassCountsRight[yl] - consecutive
                    consecutive <- 0
                    yl <- yr

                    # Calculate class probabilities
                    ClassProbLeft <- ClassCountsLeft/m
                    ClassProbRight <- ClassCountsRight/(NdSize-m)
                    # Calculate change in I based on current split
                    DeltaI <- I - sum(ClassCountsLeft*(1 - ClassProbLeft)) - sum(ClassCountsRight*(1 - ClassProbRight))

                    if (DeltaI >= MaxDeltaI){
                        # Save current best DeltaI
                        if (DeltaI != MaxDeltaI){
                            MaxDeltaI <- DeltaI
                            nBest <- 1
                            BV[nBest] <- j
                            BS[nBest] <- m
                        }else{
                            # Save all DeltaI equal to current max DeltaI
                            nBest <- nBest + 1
                            BV[nBest] <- j
                            BS[nBest] <- m
                        }
                    }
                }
            }
            # Break ties at random
            if (nBest > 1){
                BestIdx <- ceiling(runif(1,0,nBest))
                BestVar <- BV[BestIdx]
                BestSplitIdx <- BS[BestIdx]
            }else{
                BestVar <- BV[1]
                BestSplitIdx <- BS[1]
            }
            # determine split value as mean of values on either side of split
            BestSplitValue <- mean(Xnode[SortIdx[BestSplitIdx:(BestSplitIdx+1),BestVar],BestVar])
            # find which child node each sample will go to and move
            # them accordingly
            MoveLeft <- Xnode[,BestVar] <= BestSplitValue
            numMove <- length(NodeRows[[1]][MoveLeft])
            #Check to see if a split occured, or are all elements being moved one direction.
            if(numMove!=0 && numMove!=nrow(Xnode)){

                Assigned2Node <- c(Assigned2Node, list(NodeRows[[1]][MoveLeft]))
                Assigned2Node <- c(Assigned2Node, list(NodeRows[[1]][!MoveLeft]))
                
#highest Parent keeps track of the highest needed matrix and cutpoint
                    if(CurrentNode>highestParent){
                        highestParent <- CurrentNode
                    }
                    Children[CurrentNode,1] <- NextUnusedNode
                    Children[CurrentNode,2] <- NextUnusedNode+1
                    NDepth[NextUnusedNode]=NDepth[CurrentNode]+1
                    NDepth[NextUnusedNode+1]=NDepth[CurrentNode]+1
                    NodeStack <- NodeStack[-1]
                    NodeStack <- c(NextUnusedNode, NextUnusedNode+1, NodeStack)
                    NextUnusedNode <- NextUnusedNode + 2
                matA[,CurrentNode] <- sparseM[,BestVar]
                CutPoint[CurrentNode,1] <- BestSplitValue
            }else{
                NodeStack <- NodeStack[-1]
            }
            ClassProb[CurrentNode,] <- ClProb
            CurrentNode <- NodeStack[1]
            if(is.na(CurrentNode)){
                break
            }
        }
        # save current tree structure to the forest
        forest[[treeX]] <- list("CutPoint"=CutPoint[1:highestParent],"ClassProb"=ClassProb[1:(NextUnusedNode-1),],"Children"=Children[1:(NextUnusedNode-1),], "matA"=matA[,1:highestParent])

        # save error rate for this tree based on cross validation using
        # bagged values.
        #        if(bagging!=0){
        #            err <- err + error_rate(Xtest, Ytest, forest[i])
        #        }
    }
    #    if(bagging!=0){
    #        cat("Out-Of-Bag Error Rate -- ", err/trees, "\n")
    #    }
    return(forest)
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
    p <- options
    mtry <- ceiling(log2(p))
    #Create the A matrix, a sparse matrix of 1's, -1's, and 0's.
    sparseM <- matrix(0,nrow=p,ncol=mtry)
    sparseM[sample(1:(p*mtry),mtry,replace=F)] <- 1
    sparseM[sample(1:(p*mtry),mtry,replace=F)] <- -1
    #The below returns a matrix after removing zero columns in sparseM.
    sparseM <- as.matrix(sparseM[,!apply(sparseM==0,2,all)])
}


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#                       Calculate Error Rate
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
RunErr <- function(X,Y,Forest, index=0, chunk_size=0){
    if(!index || !chunk_size){
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
                rotX <- t(Z)%*%Tree$matA
                while(Tree$Children[currentNode]!=0){
                    if(rotX[currentNode]<=Tree$CutPoint[currentNode]){
                        currentNode <- Tree$Children[currentNode,1]
                    }else{
                        currentNode <- Tree$Children[currentNode,2]
                    }
                }
                classProb <- classProb + Tree$ClassProb[currentNode,]
            }
            z <- c(z,order(classProb,decreasing=T)[1])
        }
        return(sum(z!=Y))
    }else{
        X <- as.matrix(X[(((index-1)*chunk_size)+1):(index*chunk_size),])
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
                rotX <- t(Z)%*%Tree$matA
                while(Tree$Children[currentNode]!=0){
                    if(rotX[currentNode]<=Tree$CutPoint[currentNode]){
                        currentNode <- Tree$Children[currentNode,1]
                    }else{
                        currentNode <- Tree$Children[currentNode,2]
                    }
                }
                classProb <- classProb + Tree$ClassProb[currentNode,]
            }
            z <- c(z,order(classProb,decreasing=T)[1])
        }

        return(sum(z!=Y[(((index-1)*chunk_size)+1):(index*chunk_size)]))
    }
    # compare predicted to actual then divide number wrong by total tested
}


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#                          Make Predictions
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
            rotX <- t(Z)%*%Tree$matA
            while(Tree$Children[currentNode]!=0){
                if(rotX[currentNode]<=Tree$CutPoint[currentNode]){
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


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#   Run R-Rerf byte compiled and parallel                       
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
rfr <- function(X, Y, MinParent=6, trees=100, MaxDepth=0, bagging = .2, FUN=makeA, options=ncol(X), NumCores=0){

    if (!require(compiler)){
        cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to create the forest.\n")
        comp_rfr <<- runrfr

    }
    if(!exists("comp_rfr")){
        setCompilerOptions("optimize"=3)
        comp_rfr <<- cmpfun(runrfr)
    }

    if (NumCores!=1){
        if(require(parallel)){
            if(NumCores==0){
                #Use all but 1 core if NumCores=0.
                NumCores=detectCores()-1
            }
            #Start mclapply with NumCores Cores.
            if (trees%%NumCores==0){
                tree_per <- trees/NumCores
                mcrun<- function(...) comp_rfr (X, Y, MinParent, trees=tree_per, MaxDepth, bagging, FUN, options)
                forest<-do.call(c,mclapply(seq_len(NumCores), mcrun, mc.cores =NumCores, mc.set.seed=TRUE))
            }else{
                tree_per <- floor(trees/NumCores)
                mcrun<- function(...) comp_rfr (X, Y, MinParent, trees=tree_per, MaxDepth, bagging, FUN, options)
                forest<- do.call(c, mclapply(seq_len(NumCores), mcrun, mc.cores=NumCores))
                mcrun<- function(...) comp_rfr (X, Y, MinParent, trees=1, MaxDepth, bagging, FUN, options)
                forest<-c(forest,do.call(c,mclapply(seq_len(trees%%NumCores), mcrun, mc.cores=trees%%NumCores, mc.set.seed=TRUE)))
            }
        }else{
            #Parallel package not available.
            cat("Package 'parallel' not available.\nExecution will continue without parallelization.\nThis will increase the time required to create the forest\n")
            forest<-comp_rfr(X, Y, MinParent, trees, MaxDepth, bagging, FUN, options)
        }
    }else{
        #Use just one core.
        forest<-comp_rfr(X, Y, MinParent, trees, MaxDepth, bagging, FUN, options)
    }
    return(forest)
}


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#                      Run Error rate byte compiled and parallel 
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
error_rate <- function(X,Y,Forest, NumCores=0){
    if(!require(compiler)){
        cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to create the forest.\n")
        comp_err <<- RunErr
    }

    if(!exists("comp_err")){
        setCompilerOptions("optimize"=3)
        comp_err <<- cmpfun(RunErr)
    } 

    if(NumCores!=1){
        if(require(parallel)){
            if(NumCores==0){
                #Use all but 1 core if NumCores=0.
                NumCores=detectCores()-1
            }
            #Start mclapply with NumCores Cores.
            if (nrow(X)%%NumCores==0){
                chunk_size <- nrow(X)/NumCores
                comp_err_caller <- function(z, ...) comp_err(X=X,Y=Y,Forest=Forest,index=z, chunk_size=chunk_size)
                total_misclassified <- sum(as.numeric(mclapply(1:NumCores,comp_err_caller, mc.cores=NumCores)))
            }else{
                chunk_size <- floor(nrow(X)/NumCores)
                comp_err_caller <- function(z, ...) comp_err(X=X,Y=Y,Forest=Forest,index=z, chunk_size=chunk_size)
                total_misclassified <- sum(as.numeric(mclapply(1:NumCores,comp_err_caller, mc.cores=NumCores)))
                comp_err_caller <- function(z, ...) comp_err(X=X[(NumCores*chunk_size+1):nrow(X),],Y=Y[(NumCores*chunk_size+1):nrow(X)],Forest=Forest,index=z, chunk_size=1)
                total_misclassified <- total_misclassified + sum(as.numeric(mclapply(1:(nrow(X)%%NumCores), comp_err_caller, mc.cores=(nrow(X)%%NumCores))))
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
