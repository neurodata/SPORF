#' RerF forest Generator
#'
#' Creates a decision forest based on an input matrix and class vector.  This is the main function in the rerf package.
#'
#' @param X an n by d numeric matrix (preferable) or data frame. The rows correspond to observations and columns correspond to features.
#' @param Y an array of length n containing the continuous responses to be regressed on.
#' @param min.parent the minimum splittable node size.  A node size < min.parent will be a leaf node. (min.parent = 6)
#' @param trees the number of trees in the forest. (trees=100)
#' @param max.depth the longest allowable distance from the root of a tree to a leaf node (i.e. the maximum allowed height for a tree).  If max.depth=0, the tree will be allowed to grow without bound.  (max.depth=ceiling(log2(nrow(X))) )
#' @param bagging a non-zero value means a random sample of X will be used during tree creation.  If replacement = FALSE the bagging value determines the percentage of samples to leave out-of-bag.  If replacement = TRUE the non-zero bagging value is ignored. (bagging=.2)
#' @param replacement if TRUE then n samples are chosen, with replacement, from X. (replacement=FALSE)
#' @param fun a function that creates the random projection matrix. If NULL and cat.map is NULL, then RandMat is used. If NULL and cat.map is not NULL, then RandMatCat is used, which adjusts the sampling of features when categorical features have been one-of-K encoded. If a custom function is to be used, then it must return a matrix in sparse representation, in which each nonzero is an array of the form (row.index, column.index, value). See RandMat or RandMatCat for details. (fun=NULL)
#' @param mat.options a list of parameters to be used by fun. (mat.options=c(ncol(X), round(ncol(X)^.5),1L, 1/ncol(X)))
#' @param rank.transform if TRUE then each feature is rank-transformed (i.e. smallest value becomes 1 and largest value becomes n) (rank.transform=FALSE)
#' @param store.oob if TRUE then the samples omitted during the creation of a tree are stored as part of the tree.  This is required to run OOBPredict(). (store.oob=FALSE)
#' @param store.deltaI if TRUE then the decrease in impurity is stored for each split. This is required to run FeatureImportance() (store.deltaI=FALSE)
#' @param progress if TRUE then a pipe is printed after each tree is created.  This is useful for large datasets. (progress=FALSE)
#' @param rotate if TRUE then the data matrix X is uniformly randomly rotated for each tree. (rotate=FALSE)
#' @param num.cores the number of cores to use while training. If num.cores=0 then 1 less than the number of cores reported by the OS are used. (num.cores=0)
#' @param seed the seed to use for training the forest.  For two runs to match you must use the same seed for each run AND you must also use the same number of cores for each run. (seed=sample((0:100000000,1)))
#' @param cat.map a list specifying which columns in X correspond to the same one-of-K encoded feature. Each element of cat.map is a numeric vector specifying the K column indices of X corresponding to the same categorical feature after one-of-K encoding. All one-of-K encoded features in X must come after the numeric features. The K encoded columns corresponding to the same categorical feature must be placed contiguously within X. The reason for specifying cat.map is to adjust for the fact that one-of-K encoding cateogorical features results in a dilution of numeric features, since a single categorical feature is expanded to K binary features. If cat.map = NULL, then RerF assumes all features are numeric (i.e. none of the features have been one-of-K encoded).
#' @param na.action action to take if NA values are found. By default it will omit rows with NA values. NOTE: na.action is performed in-place. See default function.
#'
#' @return forest
#'
#' @examples
#' ### Train RerF on numeric data ###
#' library(rerf)
#' forest <- RerFReg(as.matrix(c(-1,0,1)), matrix(c(1,0,1,0,1,0,1,0,1), nrow = 3L), num.cores = 1L)
#'
#'
#' @export
#' @importFrom parallel detectCores mclapply makePSOCKcluster clusterEvalQ clusterSetRNGStream
#' @importFrom dummies dummy

RerFReg <-
    function(X, Y, min.parent = 6L, trees = 100L,
             max.depth = ceiling(log2(nrow(X))), bagging = .2,
             replacement = FALSE,
             fun = NULL,
             mat.options = list(p = ifelse(is.null(cat.map), ncol(X), length(cat.map)), d = ceiling(sqrt(ncol(X))), random.matrix = "binary", rho = ifelse(is.null(cat.map), 1/ncol(X), 1/length(cat.map)), prob = 0.5),
             rank.transform = FALSE, store.oob = FALSE,
             store.deltaI = TRUE, progress = FALSE,
             rotate = F, num.cores = 0L,
             na.action = function (...) { Y <<- Y[rowSums(is.na(X)) == 0];  X <<- X[rowSums(is.na(X)) == 0, ] },
             seed = sample(0:100000000,1),
						 cat.map = NULL){


        forest <- list(trees = NULL, labels = NULL, params = NULL)

        # check if data matrix X has one-of-K encoded categorical features that need to be handled specially using RandMatCat instead of RandMat
        if (is.null(fun)) {
          if (!is.null(cat.map) && !rotate) {
            fun <- RandMatCat
            mat.options[[6L]] <- cat.map
          }
          else {
            fun <- RandMat
          }
        }

        #keep from making copies of X
        if (!is.matrix(X)) {
            X <- as.matrix(X)
        }
        if (rank.transform) {
            X <- RankMatrix(X)
        }

        # address na values.
        if (any(is.na(X)) ) {
            if (exists("na.action")) na.action(X,Y)
            if (any(is.na(X))) warning("NA values exist in data matrix")
        }

        mcrun<- function(...) BuildRegTree(X, Y, min.parent, max.depth, bagging, replacement, fun, mat.options, store.oob=store.oob, store.deltaI=store.deltaI, progress=progress, rotate)

        forest$params <- list(min.parent = min.parent,
                              max.depth = max.depth,
                              bagging = bagging,
                              replacement = replacement,
                              fun = fun,
                              mat.options = mat.options,
                              rank.transform = rank.transform,
                              store.oob = store.oob,
                              store.deltaI = store.deltaI,
                              rotate = rotate,
                              seed = seed)

        if (num.cores!=1L){
            RNGkind("L'Ecuyer-CMRG")
            if(num.cores==0){
                #Use all but 1 core if num.cores=0.
                num.cores=parallel::detectCores()-1L
            }
            num.cores=min(num.cores,trees)
            gc()
            if(.Platform$OS.type=="windows"){
                cl <- parallel::makePSOCKcluster(num.cores)
                parallel::clusterSetRNGStream(cl, seed)
                parallel::clusterEvalQ(cl, library("rerf"))
                forest$trees <- parallel::parLapply(cl, 1:trees, mcrun)
                parallel::stopCluster(cl)
            }else{
            set.seed(seed)
            forest$trees <- parallel::mclapply(1:trees, mcrun, mc.cores = num.cores, mc.set.seed=TRUE)
            }
        }else{
            #Use just one core.
	    set.seed(seed)
            forest$trees <- lapply(1:trees, mcrun)
        }
        return(forest)
    }
