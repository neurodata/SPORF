rfr <- function(X, Y, mtry=0, MinParent=6, trees=100){
    #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    # BUILD_TREE builds a classification tree. Nodes are numbered from left
    # to right within a particular level (depth) of a tree. The loop over
    # nodes when considering splits is made in the same order as the
    # numbering of the nodes. That is, nodes are traversed across a single
    # level from left to right then moves to the left-most node of the next
    # level.
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
    # mtry is an integer less than p specifying the number of variables to
    # sample at each node
    #
    # MinParent is an integer specifying the minimum number of observations
    # a node must have in order for an attempt to split to be made.
    #
    # OUTPUT:
    #
    # ClassProb(i,j) is the fraction of observations in class j at node i
    #
    # CutVar(i) is the index of the feature used to split node i
    #
    # CutPoint(i) is the value of feature CutVar(i) such that an
    # observation with a value <= CutPoint(i) is assigned to the left child
    # node of node i and an observation with a value > CutPoint(i) is
    # assigned to the right child node of node i
    #
    # Parent(i) is the node index of the parent of node i
    #
    # Children(i,1) is the index of the left child node of node i and
    # Children(i,2) is the index of the right child node
    #
    # NodeSize(i) is the number of observations at node i
    #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    find_best_split <- function(Xsort,Ysort,Yunique,I,ClassCounts){

        # FIND_BEST_SPLIT does exactly that - it finds the best split variable,
        # BestVar, and the index of the observation just to the left of the best
        # split, BestSplit. Here BestSplitIdx is the row index with respect to the
        # sorted data and not to the original data. Therefore when moving the
        # data, this needs to be mapped back to the index in the original data.
        n <- nrow(Xsort)
        d <- ncol(Xsort)

        BV <- matrix(data=0, nrow=n*d, ncol= 1)
        BS <- matrix(data=0, nrow=n*d, ncol =1)
        MaxDeltaI <- 0
        nBest <- 1
        nClasses <- length(Yunique)
        ClassCountsLeft <- vector("numeric", nClasses)
        ClassCountsRight <- vector("numeric", nClasses)
        for (j in 1:d){
            consecutive <-0
            ClassCountsLeft[] <- 0
            ClassCountsRight[] <- ClassCounts
            yl <- Yunique[Ysort[1,j]]
            for (i in 1:(n-1)){
                yr <- Yunique[Ysort[i+1,j]]
                consecutive <- consecutive +1
                if (yl ==yr ){
                    next
                }

                if ( Xsort[i,j] != Xsort[i+1,j]){
                    ClassCountsLeft[yl] <- ClassCountsLeft[yl] + consecutive
                    ClassCountsRight[yl] <- ClassCountsRight[yl] - consecutive
                    consecutive <- 0             
                    yl <-yr
                    ClassProbLeft <- ClassCountsLeft/i
                    ClassProbRight <- ClassCountsRight/(n-i)
                    DeltaI <- I - sum(ClassCountsLeft*(1 - ClassProbLeft)) - sum(ClassCountsRight*(1 - ClassProbRight))

                    if (DeltaI > MaxDeltaI){
                        MaxDeltaI <- DeltaI
                        nBest <- 1
                        BV[nBest] <- j
                        BS[nBest] <- i
                    }else {
                        if (DeltaI == MaxDeltaI){
                            nBest <- nBest + 1
                            BV[nBest] <- j
                            BS[nBest] <- i
                        }
                    }
                }
            }
        }
        DeltaI <- MaxDeltaI;
        # Break ties at random
        if (nBest > 1){
            BestIdx = ceiling(runif(1,0,nBest))
            BestVar = BV[BestIdx]
            BestSplitIdx = BS[BestIdx]
        }else{
            BestVar = BV[1]
            BestSplitIdx = BS[1]
        }
        return(list("BestVar" = BestVar, "SplitValueIdx" = BestSplitIdx))
    }

    ########################################################
    ####Start main function##################################3
    if(mtry==0){
        p <- ncol(X)
        mtry=ceiling(p^.5)
    }

    Ymat <- as.matrix(Y)
    X <- as.matrix(X)
    Yunique <- as.numeric(levels(as.factor(Ymat)))    # unique class labels
    nClasses <- length(Yunique) # number of classes

    forest <- vector("list",trees)

    n <- nrow(X)
    p <- ncol(X)

    MaxNumNodes <-2*n-1    # number of tree nodes for space reservation

    ClassProb <- matrix(data=0, nrow=MaxNumNodes, ncol= nClasses)
    CutVar <- matrix(data=0, nrow=MaxNumNodes, ncol= 1)
    CutPoint <-matrix(data=0, nrow = MaxNumNodes,ncol=1)
    Parent <-matrix(data=0, nrow=MaxNumNodes,ncol=1)
    Children <-matrix(data=0, nrow = MaxNumNodes,ncol=2)
    NodeSize <-matrix(data = 0, nrow=MaxNumNodes,ncol=1)
    matA <- vector("list",MaxNumNodes)


    for(i in 1:trees){
        ClassProb[] <-0 
        CutVar[] <-0     
        CutPoint[] <-0
        Parent[] <-0
        Children[] <-0
        NodeSize[]<-0


        # Assigned2Node{i} is the set of row indices of X assigned to node i
        Assigned2Node <-list(1:n)

        NodeSize[1] <-n

        CurrentNode <- 1
        NextUnusedNode <-2

        # main loop over nodes
        while (CurrentNode < NextUnusedNode){
            NodeRows <-Assigned2Node[CurrentNode]
            NdSize <-NodeSize[CurrentNode]
            Xnode <-X[NodeRows[[1]],]
            Ynode <-Ymat[NodeRows[[1]],]

            # compute proportions of samples in each class
            ClassCounts <- matrix(data=0, nrow=1,ncol=nClasses)

            for (c in 1:nClasses){
                Cl = Yunique[c]
                ClassCounts[c] = sum(Ynode==Cl)
            }
            ClProb <-ClassCounts/NdSize
            # compute impurity for current node
            I <-NdSize*sum(ClProb*(1 - ClProb))

            # if node is impure and large enough then attempt to find good split
            if (NdSize >= MinParent && I > 0){
                #Create A matrix, a sparse matrix of 1's and 0's.
                sparseM <- matrix(0,nrow=p,ncol=mtry)
                sparseM[sample(1:(p*mtry),mtry,replace=F)]=1
                sparseM[sample(1:(p*mtry),mtry,replace=F)]=-1

                #The below gets rid of zero columns in sparseM, but sometimes gets rid of all the columns.
                sparseM <- as.matrix(sparseM[,!apply(sparseM==0,2,all)]);
                NZcols <- ncol(sparseM)

                y <- matrix(rep(Ynode,NZcols), ncol=NZcols)
                x <- matrix(data=0, nrow=NdSize, ncol=NZcols)
                #store matA to return tree specifics at end of function.
                matA[[CurrentNode]] <- sparseM

                #Xnode<- as.matrix(Xnode)%*%sparseM
                Xnode<- as.matrix(Xnode%*%sparseM)
                #Initialize a matrix to store the correct number of columns.
                if (NdSize > 1){
                    #x <-apply(Xnode,2,sort)
                    SortIdx <-apply(Xnode,2,order)
                    for (j in 1:NZcols){
                        y[,j] <-y[SortIdx[,j],j]
                        x[,j] <-Xnode[SortIdx[,j],j]
                    }
                }

                CurrentBest <- find_best_split(x,y,Yunique,I,ClassCounts)
            }else{
                CurrentBest <- list("BestVar" = 0, "SplitValueIdx" = 0)
            }

            # if good split was found, move data to left and right
            if (CurrentBest$BestVar != 0){
                BestSplitValue <- mean(x[CurrentBest$SplitValueIdx:(CurrentBest$SplitValueIdx+1),CurrentBest$BestVar])
                MoveLeft <- Xnode[,CurrentBest$BestVar] <= BestSplitValue
                Assigned2Node <- c(Assigned2Node, list(NodeRows[[1]][MoveLeft]))
                Assigned2Node <- c(Assigned2Node, list(NodeRows[[1]][!MoveLeft]))
                Children[CurrentNode,1] <- NextUnusedNode
                Children[CurrentNode,2] <- NextUnusedNode+1
                Parent[Children[CurrentNode,1]] <- CurrentNode

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

        forest[[i]]<- list("CutVar"=CutVar[1:(CurrentNode-1)],"CutPoint"=CutPoint[1:(CurrentNode-1)],"ClassProb"=ClassProb[1:(CurrentNode-1),],"NodeSize"=NodeSize[1:(CurrentNode-1)],"Parent"=Parent[1:(CurrentNode-1)],"Children"=Children[1:(CurrentNode-1),], "matA"=matA[c(1:(CurrentNode-1))])
    }
    return(forest)
}

#########################################################################
#######Calculate Error Rate
########################################################################
error_rate <- function(X,Y,Forest){
    tree_predict <- function(X,Tree){
        currentNode <- 1;
        while(Tree$Children[currentNode]!=0){
            #rotX <- X%*%Tree$matA[[currentNode]];
            rotX <- t(as.matrix(X))%*%Tree$matA[[currentNode]];
            if(rotX[Tree$CutVar[currentNode]]<Tree$CutPoint[currentNode]){
                currentNode <- Tree$Children[currentNode,1];
            }else{
                currentNode <- Tree$Children[currentNode,2];
            }
        }
        return(Tree$ClassProb[currentNode,])
    }
    ###############Main Function########################
    X <- as.matrix(X)
    Y <- as.numeric(Y)
    n <- nrow(X);
    numWrong <- 0;
    forestSize <- length(Forest)
    for(i in 1:n){
        Z <- X[i,]
        classProb<-tree_predict(Z,Forest[[1]]);
        for(j in 2:forestSize){
            classProb<- classProb+tree_predict(Z,Forest[[j]]);
        }
        z<-order(classProb,decreasing=T)[1]
        if(z!=Y[i]){
            numWrong=numWrong +1;
        }
    }
    return(numWrong/n)
}
