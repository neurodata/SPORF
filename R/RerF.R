#' RerF forest Generator
#'
#' Creates a decision forest based on an input matrix and class vector.  This is the main function in the rerf package.
#'
#' @param X an n by d numeric matrix (preferable) or data frame. The rows correspond to observations and columns correspond to features.
#' @param Y an n length vector of class labels.  Class labels must be integer or numeric and be within the range 1 to the number of classes.
#' @param FUN a function that creates the random projection matrix. If NULL and cat.map is NULL, then RandMat is used. If NULL and cat.map is not NULL, then RandMatCat is used, which adjusts the sampling of features when categorical features have been one-of-K encoded. If a custom function is to be used, then it must return a matrix in sparse representation, in which each nonzero is an array of the form (row.index, column.index, value). See RandMat or RandMatCat for details.
#' @param paramList parameters in a named list to be used by FUN.
#' @param min.parent the minimum splittable node size.  A node size < min.parent will be a leaf node. (min.parent = 6)
#' @param trees the number of trees in the forest. (trees=500)
#' @param max.depth the longest allowable distance from the root of a tree to a leaf node (i.e. the maximum allowed height for a tree).  If max.depth=0, the tree will be allowed to grow without bound.  (max.depth=ceiling(log2(nrow(X))) )
#' @param bagging a non-zero value means a random sample of X will be used during tree creation.  If replacement = FALSE the bagging value determines the percentage of samples to leave out-of-bag.  If replacement = TRUE the non-zero bagging value is ignored. (bagging=.2)
#' @param replacement if TRUE then n samples are chosen, with replacement, from X. (replacement=TRUE)
#' @param stratify if TRUE then class sample proportions are maintained during the random sampling.  Ignored if replacement = FALSE. (stratify = FALSE).
#' @param rank.transform if TRUE then each feature is rank-transformed (i.e. smallest value becomes 1 and largest value becomes n) (rank.transform=FALSE)
#' @param store.oob if TRUE then the samples omitted during the creation of a tree are stored as part of the tree.  This is required to run OOBPredict(). (store.oob=FALSE)
#' @param store.impurity if TRUE then the decrease in impurity is stored for each split. This is required to run FeatureImportance() (store.impurity=FALSE)
#' @param progress if TRUE then a pipe is printed after each tree is created.  This is useful for large datasets. (progress=FALSE)
#' @param rotate if TRUE then the data matrix X is uniformly randomly rotated for each tree. (rotate=FALSE)
#' @param num.cores the number of cores to use while training. If num.cores=0 then 1 less than the number of cores reported by the OS are used. (num.cores=0)
#' @param seed the seed to use for training the forest.  For two runs to match you must use the same seed for each run AND you must also use the same number of cores for each run. (seed=sample((0:100000000,1)))
#' @param cat.map a list specifying which columns in X correspond to the same one-of-K encoded feature. Each element of cat.map is a numeric vector specifying the K column indices of X corresponding to the same categorical feature after one-of-K encoding. All one-of-K encoded features in X must come after the numeric features. The K encoded columns corresponding to the same categorical feature must be placed contiguously within X. The reason for specifying cat.map is to adjust for the fact that one-of-K encoding cateogorical features results in a dilution of numeric features, since a single categorical feature is expanded to K binary features. If cat.map = NULL, then RerF assumes all features are numeric (i.e. none of the features have been one-of-K encoded).
#' @param rfPack boolean flag to determine whether to pack a random forest in order to improve prediction speed.  This flag is only applicable when training a forest with the "rf" option.  (rfPack = FALSE)
#'
#' @return forest
#'
#' @useDynLib rerf
#' @importFrom parallel detectCores mclapply makePSOCKcluster clusterEvalQ clusterSetRNGStream
#' @importFrom dummies dummy
#' @importFrom stats na.action
#'
#' @export
#'
#'
#' @examples
#' ### Train RerF on numeric data ###
#' library(rerf)
#' forest <- RerF(as.matrix(iris[, 1:4]), iris[[5L]], num.cores = 1L)
#'
#' ### Train RerF on one-of-K encoded categorical data ###
#' df1 <- as.data.frame(Titanic)
#' nc <- ncol(df1)
#' df2 <- df1[NULL, -nc]
#' for (i in which(df1$Freq != 0L)) {
#'   df2 <- rbind(df2, df1[rep(i, df1$Freq[i]), -nc])
#' }
#' n <- nrow(df2) # number of observations
#' p <- ncol(df2) - 1L # number of features
#' num.categories <- apply(df2[, 1:p], 2, function(x) length(unique(x)))
#' p.enc <- sum(num.categories) # number of features after one-of-K encoding
#' X <- matrix(0, nrow = n, ncol = p.enc) # initialize training data matrix X
#' cat.map <- vector("list", p)
#' col.idx <- 0L
#' # one-of-K encode each categorical feature and store in X
#' for (j in 1:p) {
#'   cat.map[[j]] <- (col.idx + 1L):(col.idx + num.categories[j])
#'   # convert categorical feature to K dummy variables
#'   X[, cat.map[[j]]] <- dummies::dummy(df2[[j]])
#'   col.idx <- col.idx + num.categories[j]
#' }
#' Y <- df2$Survived
#'
#' # specifying the cat.map in RerF allows training to
#' # be aware of which dummy variables correspond
#' # to the same categorical feature
#' forest <- RerF(X, Y, num.cores = 1L, cat.map = cat.map)
#'
#' ### Train a random rotation ensemble of CART decision trees (see Blaser and Fryzlewicz 2016)
#' forest <- RerF(as.matrix(iris[, 1:4]), iris[[5L]], num.cores = 1L,
#'                FUN = RandMatRF, paramList = list(p = 4, d = 2), rotate = TRUE)
#'

RerF <-
	function(X, Y, FUN = RandMatBinary,
           paramList = list(p = ifelse(is.null(cat.map),
                                       ncol(X),
                                       length(cat.map)),
                            d = ceiling(sqrt(ncol(X))),
                            sparsity = ifelse(is.null(cat.map),
                                              1/ncol(X),
                                              1/length(cat.map)),
                            prob = 0.5),
           min.parent = 6L, trees = 500L,
					 max.depth = ceiling(log2(nrow(X))), bagging = .2,
					 replacement = TRUE, stratify = FALSE,
					 rank.transform = FALSE, store.oob = FALSE,
					 store.impurity = FALSE, progress = FALSE,
					 rotate = FALSE, num.cores = 0L,
					 seed = sample(0:100000000,1),
					 cat.map = NULL, rfPack = FALSE,
					 task = 'classification'){

    FUN <- match.fun(FUN, descend = TRUE)

    #keep from making copies of X
    if (!is.matrix(X)) {
      X <- as.matrix(X)
    }
    if (rank.transform) {
      X <- RankMatrix(X)
    }

    if (task == "classification") {
      task_inputs <- preprocess.classification(X, Y, cat.map, stratify, paramList)
      paramList <- task_inputs$paramList
      forest <- task_inputs$forest

      mcrun <- function(...) {
        ClassificationTree(
          X              = task_inputs$X,
          Y              = task_inputs$Y,
          FUN            = FUN,
          paramList      = paramList,
          min.parent     = min.parent,
          max.depth      = max.depth,
          bagging        = bagging,
          replacement    = replacement,
          stratify       = stratify,
          class.ind      = task_inputs$Cindex,
          class.ct       = task_inputs$classCt,
          store.oob      = store.oob,
          store.impurity = store.impurity,
          progress       = progress,
          rotate         = rotate
        )
      }

      forest$params <- list(min.parent = min.parent,
                            max.depth = max.depth,
                            bagging = bagging,
                            replacement = replacement,
                            stratify = stratify,
                            fun = FUN,
                            paramList = paramList,
                            rank.transform = rank.transform,
                            store.oob = store.oob,
                            store.impurity = store.impurity,
                            rotate = rotate,
                            seed = seed)
    } else if (task == "regression") {
      task_inputs <- preprocess.regression(X,Y)

      mcrun <- function(...) {
        RegressionTree(
          X              = task_inputs$X,
          Y              = task_inputs$Y,
          FUN            = FUN,
          paramList      = paramList,
          min.parent     = min.parent,
          max.depth      = max.depth,
          bagging        = bagging,
          replacement    = replacement,
          store.oob      = store.oob,
          store.impurity = store.impurity,
          progress       = progress,
          rotate         = rotate
        )
      }

      forest$params <- list(min.parent = min.parent,
                            max.depth = max.depth,
                            bagging = bagging,
                            replacement = replacement,
                            stratify = stratify,
                            fun = FUN,
                            paramList = paramList,
                            rank.transform = rank.transform,
                            store.oob = store.oob,
                            store.impurity = store.impurity,
                            rotate = rotate,
                            seed = seed)

    } else {
      stop("Invalid task specified")
    }

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
			} else {
				set.seed(seed)
				forest$trees <- parallel::mclapply(1:trees, mcrun, mc.cores = num.cores, mc.set.seed = TRUE)
			}
		} else {
			#Use just one core.
			set.seed(seed)
			forest$trees <- lapply(1:trees, mcrun)
		}
		if(identical(FUN, rerf::RandMatRF) & rfPack) {
			PackForest(X,Y,forest)
		}
		return(forest)
	}

