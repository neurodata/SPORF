#function [CutVar,CutPoint,ClassProb,NodeSize,Parent,Children] = build_tree(X,Y,mtry,MinParent)
build_tree <- function(X, Y, mtry, MinParent){

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

    # compute number of observations n and number of features p
    n <- nrow(X);
    p <- ncol(X);

    Ymat <- as.matrix(Y);
    Yunique <- levels(as.factor(Ymat));    # unique class labels
    nClasses <- length(Yunique); # number of classes

    MaxNumNodes <-2*n-1;    # number of tree nodes for space reservation

    ClassProb <- matrix(data=0, nrow=MaxNumNodes, ncol= nClasses);
    CutVar <- matrix(data=0, nrow=MaxNumNodes, ncol= 1);
    CutPoint <-matrix(data=0, nrow = MaxNumNodes,ncol=1);
    Parent <-matrix(data=0, nrow=MaxNumNodes,ncol=1);
    Children <-matrix(data=0, nrow = MaxNumNodes,ncol=2);
    NodeSize <-matrix(data = 0, nrow=MaxNumNodes,ncol=1);

    # Assigned2Node{i} is the set of row indices of X assigned to node i
    Assigned2Node <-list(1:n);

    NodeSize[1] <-n;

    CurrentNode <- 1;
    NextUnusedNode <-2;

    # main loop over nodes
    while (CurrentNode < NextUnusedNode){
        NodeRows <-Assigned2Node[CurrentNode];
        NdSize <-NodeSize[CurrentNode];
        Xnode <-X[NodeRows[[1]],];
        Ynode <-Ymat[NodeRows[[1]],];

        # compute proportions of samples in each class
        ClassCounts <- matrix(data=0, nrow=1,ncol=nClasses);

        for (c in 1:nClasses){
            Cl = Yunique[c];
            ClassCounts[c] = sum(Ynode==Cl);
        }

        ClProb <-ClassCounts/NdSize;
        # compute impurity for current node
        I <-NdSize*sum(ClProb*(1 - ClProb));
        # if node is impure and large enough then attempt to find good split
        if (NdSize >= MinParent && I > 0){
            # y = rep(Ynode,1,mtry);#This doesn't look right -- look at Breiman's and ask Tyler.
            y <- matrix(rep(Ynode,mtry), ncol=mtry);
            VarSample <- sample(p,mtry);

            Xnode<- Xnode[,VarSample];
            if (NdSize > 1){
                x <-apply(Xnode,2,sort);
                SortIdx <-apply(Xnode,2,order);
                for (j in 1:mtry){
                    y[,j] <-y[SortIdx[,j],j];
                }
            }

            CurrentBest <- find_best_split(x,y,Yunique,I,ClassCounts);
        }else{
            CurrentBest <- list("BestVar" = 0, "SplitValueIdx" = 0);
        }

        # if good split was found, move data to left and right
        if (CurrentBest$BestVar != 0){
            BestSplitValue <- mean(x[CurrentBest$SplitValueIdx:(CurrentBest$SplitValueIdx+1),CurrentBest$BestVar]);
            MoveLeft <- Xnode[,CurrentBest$BestVar] <= BestSplitValue;
            Assigned2Node <- c(Assigned2Node, list(NodeRows[[1]][MoveLeft]));
            Assigned2Node <- c(Assigned2Node, list(NodeRows[[1]][!MoveLeft]));
            Children[CurrentNode,1] <- NextUnusedNode;
            Children[CurrentNode,2] <- NextUnusedNode+1;
            Parent[Children[CurrentNode,1]] <- CurrentNode;

            NodeSize[NextUnusedNode,1] <- length(Assigned2Node[[NextUnusedNode]]);
            NodeSize[NextUnusedNode+1,1] <- length(Assigned2Node[[NextUnusedNode+1]]);

            NextUnusedNode <- NextUnusedNode + 2;

            CutVar[CurrentNode,1] <- VarSample[CurrentBest$BestVar];
            CutPoint[CurrentNode,1] <- BestSplitValue;
        }else{
            CutVar[CurrentNode,1] <- 0;
            CutPoint[CurrentNode,1] <- 0;
        }

        ClassProb[CurrentNode,] <- ClProb;

        CurrentNode <- CurrentNode + 1;
    }

    CutVar <- CutVar[1:(CurrentNode-1)];
    CutPoint <- CutPoint[1:(CurrentNode-1)];
    ClassProb <- ClassProb[1:(CurrentNode-1),];
    NodeSize <- NodeSize[1:(CurrentNode-1)];
    Parent <- Parent[1:(CurrentNode-1)];
    Children <- Children[1:(CurrentNode-1),];

    return(list("CutVar"=CutVar,"CutPoint"=CutPoint,"ClassProb"=ClassProb,"NodeSize"=NodeSize,"Parent"=Parent,"Children"=Children));
}

#function [BestVar,BestSplitIdx,DeltaI] = find_best_split(Xsort,Ysort,Yunique,I,ClassCounts)
find_best_split <- function(Xsort,Ysort,Yunique,I,ClassCounts){

    # FIND_BEST_SPLIT does exactly that - it finds the best split variable,
    # BestVar, and the index of the observation just to the left of the best
    # split, BestSplit. Here BestSplitIdx is the row index with respect to the
    # sorted data and not to the original data. Therefore when moving the
    # data, this needs to be mapped back to the index in the original data.
    n <- nrow(Xsort);
    d <- ncol(Xsort);

    BV <- matrix(data=0, nrow=n*d, ncol= 1);
    BS <- matrix(data=0, nrow=n*d, ncol =1);
    MaxDeltaI <- 0;
    nBest <- 1;
    nClasses <- length(Yunique);

    #yfac<-Ysort;
    #levs<-levels(yfac);
    #yfac<-matrix(data=as.numeric(yfac), ncol=d);

    for (j in 1:d){

        ClassCountsLeft <- matrix(data=0, nrow=1, ncol=nClasses);
        ClassCountsRight <- matrix(data=ClassCounts, nrow=1, ncol=nClasses);

        for (i in 1:(n-1)){
            yl <- as.numeric(Yunique[Ysort[i,j]]);
            yr <- as.numeric(Yunique[Ysort[i+1,j]]);
            ClassCountsLeft[yl] <- ClassCountsLeft[yl] + 1;
            ClassCountsRight[yl] <- ClassCountsRight[yl] - 1;

            if (Xsort[i+1,j] != Xsort[i,j] && yl != yr){
                ClassProbLeft <- ClassCountsLeft/i;
                ClassProbRight <- ClassCountsRight/(n-i);
                DeltaI <- I - sum(ClassCountsLeft*(1 - ClassProbLeft)) - sum(ClassCountsRight*(1 - ClassProbRight));

                if (DeltaI > MaxDeltaI){
                    MaxDeltaI <- DeltaI;
                    nBest <- 1;
                    BV[nBest] <- j;
                    BS[nBest] <- i;
                }else {
                    if (DeltaI == MaxDeltaI){
                        nBest <- nBest + 1;
                        BV[nBest] <- j;
                        BS[nBest] <- i;
                    }
                }
            }
        }
    }
    DeltaI <- MaxDeltaI;
    # Break ties at random
    if (nBest > 1){
        BestIdx = ceiling(runif(1,0,nBest));
        BestVar = BV[BestIdx];
        BestSplitIdx = BS[BestIdx];
    }else{
        BestVar = BV[1];
        BestSplitIdx = BS[1];
    }
    return(list("BestVar" = BestVar, "SplitValueIdx" = BestSplitIdx));
}
