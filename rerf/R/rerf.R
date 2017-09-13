#' RerF Forest Generator
#'
#' Creates a decision forest based on an input matrix and class vector.  This is the main function in the rerf package.
#'
#' @param X an n by d numeric matrix (preferable) or data frame. The rows correspond to observations and columns correspond to features.
#' @param Y an n length vector of class labels.  Class labels must be integer or numeric and be within the range 1 to the number of classes.
#' @param num.class an integer specifying the number of unique class labels. This should be the number of possibly observable class labels, even if only a subset is actually observed in Y.
#' @param min.parent the minimum splittable node size.  A node size < min.parent will be a leaf node. (min.parent = 6)
#' @param trees the number of trees in the forest. (trees=100)
#' @param max.depth the longest allowable distance from the root of a tree to a leaf node (i.e. the maximum allowed height for a tree).  If max.depth=0, the tree will be allowed to grow without bound.  (max.depth=0)  
#' @param bagging a non-zero value means a random sample of X will be used during tree creation.  If replacement = FALSE the bagging value determines the percentage of samples to leave out-of-bag.  If replacement = TRUE the non-zero bagging value is ignored. (bagging=.2) 
#' @param replacement if TRUE then n samples are chosen, with replacement, from X. (replacement=TRUE)
#' @param stratify if TRUE then class sample proportions are maintained during the random sampling.  Ignored if replacement = FALSE. (stratify = FALSE).
#' @param fun a function that creates the random projection matrix. (fun=makeA) 
#' @param options a list of parameters to be used by fun. (options=c(ncol(X), round(ncol(X)^.5),1L, 1/ncol(X)))
#' @param rank.transform if TRUE then each feature is rank-transformed (i.e. smallest value becomes 1 and largest value becomes n) (rank.transform=FALSE)
#' @param store.oob if TRUE then the samples omitted during the creation of a tree are stored as part of the tree.  This is required to run OOBPredict(). (store.oob=FALSE)
#' @param store.ns if TRUE then the number of training observations at each node is stored. This is required to run FeatureImportance() (store.ns=FALSE)
#' @param progress if TRUE then a pipe is printed after each tree is created.  This is useful for large datasets. (progress=FALSE)
#' @param rotate if TRUE then the data matrix X is uniformly randomly rotated for each tree. (rotate=FALSE)
#' @param num.cores the number of cores to use while training. If num.cores=0 then 1 less than the number of cores reported by the OS are used. (num.cores=0)
#' @param seed the seed to use for training the forest. (seed=1)
#'
#' @return Forest
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#' 
#' @examples
#' library(rerf)
#' trainedForest <- RerF(as.matrix(iris[,1:4]), as.numeric(iris[,5]), num.cores=1)
#'
#' @export
#' @importFrom compiler setCompilerOptions cmpfun
#' @importFrom parallel detectCores mclapply mc.reset.stream
#'

RerF <-
    function(X, Y, num.class, min.parent=6L, trees=100L, max.depth=0L, bagging = .2, replacement=TRUE, stratify=FALSE, fun=makeA, options=c(ncol(X), round(ncol(X)^.5),1L, 1/ncol(X)), rank.transform = FALSE, store.oob=FALSE, store.ns=FALSE, progress=FALSE, rotate = F, num.cores=0L, seed = 1L){

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
        classCt <- tabulate(Y, num.class)
        for(q in 2:num.class){
          classCt[q] <- classCt[q]+classCt[q-1L]
        }
        if(stratify){
          Cindex<-vector("list",num.class)
          for(m in 1L:num.class){
            Cindex[[m]]<-which(Y==m)
          }
        }else{
          Cindex<-NULL
        }

        compiler::setCompilerOptions("optimize"=3)
        comp_tree <- compiler::cmpfun(BuildTree)

        mcrun<- function(...) comp_tree(X, Y, min.parent, max.depth, bagging, replacement, stratify, Cindex, classCt, fun, options, store.oob=store.oob, store.ns=store.ns, progress=progress, rotate)

        if (num.cores!=1L){
            RNGkind("L'Ecuyer-CMRG")
            set.seed(seed)
            parallel::mc.reset.stream()
            if(num.cores==0){
                #Use all but 1 core if num.cores=0.
                num.cores=parallel::detectCores()-1L
            }
            num.cores=min(num.cores,trees)
            gc()
            forest <- parallel::mclapply(1:trees, mcrun, mc.cores = num.cores, mc.set.seed=TRUE)
        }else{
            #Use just one core.

            forest <- lapply(1:trees, mcrun, mc.cores = num.cores, mc.set.seed=TRUE)
        }
        return(forest)
    }
