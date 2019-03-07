#' RerF forest Generator
#'
#' Creates a decision forest based on an input matrix and class vector.  This is the main function in the rerf package.
#'
#' @param X an n by d numeric matrix (preferable) or data frame. The rows correspond to observations and columns correspond to features.
#' @param Y an n length vector of class labels.  Class labels must be integer or numeric and be within the range 1 to the number of classes.
#' @param FUN a function that creates the random projection matrix. If NULL and cat.map is NULL, then RandMat is used. If NULL and cat.map is not NULL, then RandMatCat is used, which adjusts the sampling of features when categorical features have been one-of-K encoded. If a custom function is to be used, then it must return a matrix in sparse representation, in which each nonzero is an array of the form (row.index, column.index, value). See RandMat or RandMatCat for details.
#' @param paramList parameters in a named list to be used by FUN. If left unchanged,
#' default values will be populated, see \code{\link[rerf]{defaults}} for details.
#' @param min.parent the minimum splittable node size.  A node size < min.parent will be a leaf node. (min.parent = 1)
#' @param trees the number of trees in the forest. (trees=500)
#' @param max.depth the longest allowable distance from the root of a tree to a leaf node (i.e. the maximum allowed height for a tree).  If max.depth=0, the tree will be allowed to grow without bound.  (max.depth=0)
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
#' @param task string specifies whether 'classification' or 'similarity'
#' should be run.
#' @param rfPack boolean flag to determine whether to pack a random forest in order to improve prediction speed.  This flag is only applicable when training a forest with the "rf" option.  (rfPack = FALSE)
#' @param eps a scalar between 0 and 1. A leaf node is designated if the mean node similarity is at least 1 - eps. Only used if task is 'similarity' (eps=0.05)
#'
#' @return forest
#'
#' @useDynLib rerf
#' @import Rcpp
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
#' forest <- RerF(as.matrix(iris[, 1:4]), as.numeric(iris[[5L]]), num.cores = 1L)
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
#' \dontrun{
#' # takes longer than 5s to run.
#' # adding a continuous feature along with the categorical features
#' # must be prepended to the categorical features.
#' set.seed(1234)
#' xp <- runif(nrow(X))
#' Xp <- cbind(xp, X)
#' cat.map1 <- lapply(cat.map, function(x) x + 1)
#' forestW <- RerF(Xp, Y, num.cores = 1L, cat.map = cat.map1)
#' }
#'
#' ### Train a random rotation ensemble of CART decision trees (see Blaser and Fryzlewicz 2016)
#' forest <- RerF(as.matrix(iris[, 1:4]), iris[[5L]],
#'   num.cores = 1L,
#'   FUN = RandMatRF, paramList = list(p = 4, d = 2), rotate = TRUE
#' )
#'
#' ### Train a SmerF forest
#'
#' X <- iris[, -5]
#' Y <- 1 - as.matrix(dist(X))
#'
#' iris.forest <- RerF(X, Y, task = 'similarity', num.cores = 2L)
#' Yhat <- Predict(X, iris.forest, num.cores = 2L)
#'
#' ### Frobenius norm of $Y - \hat{Y}$
#' (f.iris <- norm(Y - Yhat, "F"))
#'
#' par(mfrow = c(1,3))
#' image(Y - Yhat, col = gray.colors(255), main = "Y - Yhat")
#' image(Yhat, col = gray.colors(255), main = "Yhat")
#' image(Y, col = gray.colors(255), main = "Y")
#'


RerF <-
  function(X, Y, FUN = RandMatBinary,
             paramList = list(p = NA, d = NA, sparsity = NA, prob = NA),
             min.parent = 1L, trees = 500L,
             max.depth = 0, bagging = .2,
             replacement = TRUE, stratify = TRUE,
             rank.transform = FALSE, store.oob = FALSE,
             store.impurity = FALSE, progress = FALSE,
             rotate = FALSE, num.cores = 0L,
             seed = sample(0:100000000, 1),
             cat.map = NULL, rfPack = FALSE,
             task = "classification", eps = 0.05) {

    # The below 'na.action' was removed from the parameter list of RerF because the CRAN check did not accept it and because it will potentially change the X and Y input by the user.
    # na.action = function (...) { Y <<- Y[rowSums(is.na(X)) == 0];  X <<- X[rowSums(is.na(X)) == 0, ] },
    # @param na.action action to take if NA values are found. By default it will omit rows with NA values. NOTE: na.action is performed in-place. See default function.


    paramList <- defaults(ncolX = ncol(X), paramList = paramList, cat.map = cat.map)
    FUN <- match.fun(FUN, descend = TRUE)

    forest <- list(trees = NULL, labels = NULL, params = NULL)

    # check if data matrix X has one-of-K encoded categorical features that
    # need to be handled specially using RandMatCat instead of RandMat

    if (!is.null(cat.map)) {
      paramList$catMap <- cat.map
    }

    # keep from making copies of X
    if (!is.matrix(X)) {
      X <- as.matrix(X)
    }
    if (rank.transform) {
      X <- RankMatrix(X)
    }

    # check if the data type of Y matches the learning task
    if (task == "classification") {
      if (is.matrix(Y)) {
        if ((dim(Y)[1L] == 1L) || (dim(Y)[2L] == 1L)) {
          Y <- as.vector(Y)
        } else {
          stop("Incompatible data type. Y must be a vector or 1D matrix")
        }
      }
      # adjust Y to go from 1 to num.class if needed
      if (is.factor(Y)) {
        forest$labels <- levels(Y)
        Y <- as.integer(Y)
      } else if (is.numeric(Y)) {
        forest$labels <- sort(unique(Y))
        Y <- as.integer(as.factor(Y))
      } else {
        stop("Incompatible data type. Y must be of type factor or numeric.")
      }
      num.class <- length(forest$labels)
      classCt <- cumsum(tabulate(Y, num.class))
      if (stratify) {
        Cindex <- vector("list", num.class)
        for (m in 1L:num.class) {
          Cindex[[m]] <- which(Y == m)
        }
      } else {
        Cindex <- NULL
      }
    } else if (task == "similarity") {
      if (!is.matrix(Y)) {
        stop("Incompatible data type. Y must be an n-by-n numeric matrix.")
      } else {
        if (!is.numeric(Y[1L])) {
          stop("Incompatible data type. Y must be an n-by-n numeric matrix.")
        }
        if (!identical(Y, t(Y))) {
          stop("Incompatible data type. Y must be a symmetric numeric matrix.")
        }
      }
    }

    # address na values.
    if (any(is.na(X))) {
      if (exists("na.action")) {
        stats::na.action(X, Y)
      }
      if (any(is.na(X))) {
        warning("NA values exist in data matrix")
      }
    }

    mcrun <- function(...) {
      if (task == "classification") {
        BuildTree(
          X = X,
          Y = Y,
          FUN = FUN,
          paramList = paramList,
          min.parent = min.parent,
          max.depth = max.depth,
          bagging = bagging,
          replacement = replacement,
          stratify = stratify,
          class.ind = Cindex,
          class.ct = classCt,
          store.oob = store.oob,
          store.impurity = store.impurity,
          progress = progress,
          rotate = rotate
        )
      } else if (task == "similarity") {
        BuildSimTree(
          X = X,
          Y = Y,
          FUN = FUN,
          paramList = paramList,
          min.parent = min.parent,
          max.depth = max.depth,
          bagging = bagging,
          replacement = replacement,
          stratify = stratify,
          store.oob = store.oob,
          store.impurity = store.impurity,
          progress = progress,
          rotate = rotate,
          eps = eps
        )
      }
    }

    forest$params <- list(
      min.parent = min.parent,
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
      seed = seed,
      task = task,
      eps = if (task == "similarity") {
        eps
      }
    )

    if (num.cores != 1L) {
      RNGkind("L'Ecuyer-CMRG")
      if (num.cores == 0) {
        # Use all but 1 core if num.cores=0.
        num.cores <- parallel::detectCores() - 1L
      }
      num.cores <- min(num.cores, trees)
      gc()
      if (.Platform$OS.type == "windows") {
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
      # Use just one core.
      set.seed(seed)
      forest$trees <- lapply(1:trees, mcrun)
    }
    if (identical(FUN, rerf::RandMatRF) & rfPack) {
      PackForest(X, Y, forest)
    }
    return(forest)
  }
