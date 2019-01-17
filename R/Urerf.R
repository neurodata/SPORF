#' Unsupervised RerF forest Generator
#'
#' Creates a decision forest based on an input matrix.
#'
#' @param X an n by d numeric matrix. The rows correspond to observations and columns correspond to features.
#' @param trees the number of trees in the forest. (trees=100)
#' @param min.parent the minimum splittable node size.  A node size < min.parent will be a leaf node. (min.parent = round(nrow(X)^.5))
#' @param max.depth the longest allowable distance from the root of a tree to a leaf node (i.e. the maximum allowed height for a tree).  If max.depth=NA, the tree will be allowed to grow without bound.  (max.depth=NA)
#' @param mtry the number of features to test at each node.  (mtry=ceiling(ncol(X)^.5))
#' @param normalizeData a logical value that determines if input data is normalized to values ranging from 0 to 1 prior to processing.  (normalizeData=TRUE)
#' @param sparsity a real number in \eqn{(0,1)} that specifies the distribution of non-zero elements in the random matrix. (sparsity=1/nrow(X))
#' @param Progress boolean for printing progress.
#' @param splitCrit split based on twomeans(splitCrit="twomeans") or BIC test(splitCrit="BIC")#
#'
#' @return urerfStructure
#'
#' @export
#'
#'
#' @examples
#' ### Train RerF on numeric data ###
#' library(rerf)
#' urerfStructure <- Urerf(as.matrix(iris[, 1:4]))
#'
#' dissimilarityMatrix <- hclust(as.dist(1 - urerfStructure$similarityMatrix), method = "mcquitty")
#' clusters <- cutree(dissimilarityMatrix, k = 3)
library(mclust)
Urerf <- function(X, trees = 100, min.parent = round(nrow(X)^0.5),
                  max.depth = NA, mtry = ceiling(ncol(X)^0.5),
									sparsity = 1 / ncol(X),
                  normalizeData = TRUE, Progress = TRUE, 
                  splitCrit = "twomeans", LinearCombo = TRUE) {
  normalizeTheData <- function(X, normData) {
    if (normData) {
      X <- sweep(X, 2, apply(X, 2, min), "-")
      return(sweep(X, 2, apply(X, 2, max), "/"))
    } else {
      return(X)
    }
  }

  normalizeDataInfo <- function(X, normData) {
    if (normData) {
      colMin <- apply(X, 2, min)
      colMax <- apply(sweep(X, 2, apply(X, 2, min)), 2, max)
    } else {
      colMin <- 0
      colMax <- 1
    }
    list(colMin = colMin, colMax = colMax)
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
            simMatrix[Forest[[i]]$ALeaf[[j]][k], Forest[[i]]$ALeaf[[j]][iterator]] <-
              simMatrix[Forest[[i]]$ALeaf[[j]][k], Forest[[i]]$ALeaf[[j]][iterator]] + 1
          }
        }
      }
    }
    simMatrix <- simMatrix / numTrees
    if (any(diag(simMatrix) != 1)) {
      print("diag not zero")
      diag(simMatrix) <- 1
    }
    return(simMatrix)
  }

  ########### Start Urerf #############
  K <- min.parent
  numTrees <- trees
  depth <- max.depth
  checkInputMatrix(X)

  normInfo <- normalizeDataInfo(X, normalizeData)
  X <- normalizeTheData(X, normalizeData)

  forest <- if (is.na(depth)) {
    GrowUnsupervisedForest(X, trees = numTrees, MinParent = K, options = list(
      p = ncol(X),
      d = mtry, sparsity = sparsity
    ), Progress = Progress, splitCrit = splitCrit, LinearCombo = LinearCombo)
  } else {
    GrowUnsupervisedForest(X,
      trees = numTrees, MinParent = K, MaxDepth = depth,
      options = list(p = ncol(X), d = mtry, sparsity = sparsity), 
      Progress = Progress, splitCrit = splitCrit, LinearCombo = LinearCombo)
  }

  sM <- createMatrixFromForest(forest)

  outliers <- apply(sM, 1, function(x) sum(sort(x, decreasing = TRUE)[1:3]))

  outlierMean <- mean(outliers)
  outlierSD <- sd(outliers)

  return(list(
    similarityMatrix = sM, forest = forest, colMin = normInfo$colMin,
    colMax = normInfo$colMax, outlierMean = outlierMean, outlierSD = outlierSD,
    trainSize = nrow(X)
  ))
}
