#' Unsupervised Lumberjack
#'
#' Creates the forest and supporting data that makes up the uLumberjack structure. At its core this function is a wrapper for the GrowUnsupervisedForest function.
#'
#' @param X an n by d numeric matrix (preferable) or data frame. The rows correspond to observations and columns correspond to features.  X must be numeric or convertable to numeric.
#' @param trees the number of trees to grow (trees = 100).
#' @param K the minimum leaf node size.  Use of K increases liklihood that nodes will contain multiple observations. (K = 10)
#' @param depth the longest allowable distance from the root of a tree to a leaf node (i.e. the maximum allowed height for a tree).  If depth=NA then K is used as the sole stopping criteria. (depth=NA)
#'
#' @return urerfStructure
#'
#' @examples
#' library(rerf)
#' X <- as.matrix(iris[,1:4])
#' urerfStructure <- UnsupervisedLumberjack(X, trees=100, K=10, depth=5)
#'
#' @export
#'


UnsupervisedLumberjack <- function(X, trees = 100, min.parent = 10,
                                   max.depth = NA) {
  k <- min.parent
  depth <- max.depth
  numTrees <- trees

  ######## Define Helper functions ########
  normalizeData <- function(X) {
    X <- sweep(X, 2, apply(X, 2, min), "-")
    sweep(X, 2, apply(X, 2, max), "/")
  }

  normalizeDataInfo <- function(X) {
    colMin <- apply(X, 2, min)
    colMax <- apply(sweep(X, 2, apply(X, 2, min)), 2, max)
    list(colMin = colMin, colMax = colMax)
  }

  checkInputMatrix <- function(X) {
    if (is.null(X)) {
      stop("the input is null.")
    }
    if (sum(is.na(X)) | sum(is.nan(X))) {
      stop("some values are na or nan.")
    }
    if (sum(colSums(X) == 0) != 0) {
      stop("some columns are all zero.")
    }
  }

  createMatrixFromForest <- function(Forest) {
    tS <- Forest[[1]]$TrainSize
    numTrees <- length(Forest)

    simMatrix <- matrix(0, nrow = tS, ncol = tS)

    for (i in 1:numTrees) {
      childNodes <- which(Forest[[i]]$Children[, 1] == 0)
      for (j in childNodes) {
        for (k in 1:length(Forest[[i]]$ALeaf[[j]])) {
          for (iterator in 1:length(Forest[[i]]$ALeaf[[j]])) {
          simMatrix[Forest[[i]]$ALeaf[[j]][k], Forest[[i]]$ALeaf[[j]][iterator]] <- simMatrix[Forest[[i]]$ALeaf[[j]][k],
            Forest[[i]]$ALeaf[[j]][iterator]] + 1
          }
        }
      }
    }
    simMatrix <- simMatrix/tS
    diag(simMatrix) <- 1
    return(simMatrix)
  }

  ########### Start Urerf #############
  checkInputMatrix(X)

  normInfo <- normalizeDataInfo(X)
  X <- normalizeData(X)

  forest <- invisible(ifelse(is.na(depth), GrowUnsupervisedForest(X, trees = numTrees,
    MinParent = K), GrowUnsupervisedForest(X, trees = numTrees, MaxDepth = depth)))
  sM <- createMatrixFromForest(forest)

  outliers <- apply(sM, 1, function(x) sum(sort(x, decreasing = TRUE)[1:3]))

  outlierMean <- mean(outliers)
  outlierSD <- sd(outliers)
  print(" ")

  return(list(similarityMatrix = sM, forest = forest, colMin = normInfo$colMin,
    colMax = normInfo$colMax, outlierMean = outlierMean, outlierSD = outlierSD,
    trainSize = nrow(X)))
}
