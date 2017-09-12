#' RerF Forest Generator
#'
#' Creates a decision forest based on an input matrix and class vector.  This is the main function in the rerf package.
#'
#' @param X an n sample by d feature matrix (preferable) or data frame
#' @param Y an n length vector of class labels.  Class labels must be numeric and be within the range 1 to the number of classes.
#' @param MinParent the minimum splittable node size.  A node size < MinParent will be a leaf node. (MinParent = 6)
#' @param trees the number of trees in the forest. (trees=100)
#' @param MaxDepth the longest allowable distance from the root of a tree to a leaf node (i.e. the maximum allowed height for a tree).  If MaxDepth=0, the tree will be allowed to grow without bound.  (MaxDepth=0)  
#' @param bagging a non-zero value means a random sample of X will be used during tree creation.  If replacement = FALSE the bagging value determines the percentage of samples to leave out-of-bag.  If replacement = TRUE the non-zero bagging value is ignored. (bagging=.2) 
#' @param replacement if true then n samples are chosen, with replacement, from X. (replacement=TRUE)
#' @param stratify if true then class sample proportions are maintained during the random sampling.  Ignored if replacement = FALSE. (stratify = FALSE).
#' @param FUN a function that creates the random projection matrix. (FUN=makeA) 
#' @param options a list of parameters to be used by FUN. (options=c(ncol(X), round(ncol(X)^.5),1L, 1/ncol(X)))
#' @param rank.transform ????? (rank.transform=FALSE)
#' @param COOB if true the samples omitted during the creation of a tree are stored as part of the tree.  This is required to run the OOB function. (COOB=FALSE)
#' @param CNS ????? (CNS=FALSE)
#' @param Progress if true a pipe is printed after each tree is created.  This is useful for large datasets. (Progress=FALSE)
#' @param rotate ????? (rotate=FALSE)
#' @param NumCores the number of cores to use while training. If NumCores=0 then 1 less than the number of cores reported by the OS are used. (NumCores=0)
#' @param seed the seed to use for training the forest. (seed=1)
#'
#' @return Forest
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#' 
#' @examples
#' rerf(as.matrix(iris[,1:4]), as.numeric(iris[,5]))
#'
#' @export
#' @importFrom compiler setCompilerOptions cmpfun
#' @importFrom parallel detectCores mclapply mc.reset.stream
#'

rerf <-
    function(X, Y, MinParent=6L, trees=100L, MaxDepth=0L, bagging = .2, replacement=TRUE, stratify=FALSE, FUN=makeA, options=c(ncol(X), round(ncol(X)^.5),1L, 1/ncol(X)), rank.transform = FALSE, COOB=FALSE, CNS=FALSE, Progress=FALSE, rotate = F, NumCores=0L, seed = 1L){

        #keep from making copies of X

        if (!is.matrix(X)) {
            X <- as.matrix(X)
        }
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

        compiler::setCompilerOptions("optimize"=3)
        comp_tree <- compiler::cmpfun(BuildTree)

        mcrun<- function(...) comp_tree(X, Y, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB=COOB, CNS=CNS, Progress=Progress, rotate)

        if (NumCores!=1L){
            RNGkind("L'Ecuyer-CMRG")
            set.seed(seed)
            parallel::mc.reset.stream()
            if(NumCores==0){
                #Use all but 1 core if NumCores=0.
                NumCores=parallel::detectCores()-1L
            }
            NumCores=min(NumCores,trees)
            gc()
            forest <- parallel::mclapply(1:trees, mcrun, mc.cores = NumCores, mc.set.seed=TRUE)
        }else{
            #Use just one core.

            forest <- lapply(1:trees, mcrun, mc.cores = NumCores, mc.set.seed=TRUE)
        }
        return(forest)
    }
