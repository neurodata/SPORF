#' RerF Tree Printer
#'
#' Prints the layout of a specified tree.
#'
#' @param forest a rerf forest structure.
#' @param numTree the tree number to print. (numTree=1)
#'
#' @export
#'
#' @examples
#' ### Train RerF on numeric data ###
#' library(rerf)
#' forest <- rerf(as.matrix(iris[, 1:4]), iris[, 5])
#' 
#' PrintTree(forest, 1)
PrintTree <- function(forest, numTree = 1) {

  # TODO: make forest a class so that the input can be tested prior to processing.

  # hacky test to make sure forest is of correct type
  if (!all(c("trees", "labels", "params") %in% names(forest))) {
    warning("forest not of correct type.")
    return()
  }

  if (numTree < 1 | numTree > length(forest$trees)) {
    warning("numTree not within acceptable range.")
    return()
  }

  nodeNum <- vector("integer", length(forest$trees[[numTree]]$treeMap))
  LC <- nodeNum <- vector("integer", length(forest$trees[[numTree]]$treeMap))
  RC <- nodeNum <- vector("integer", length(forest$trees[[numTree]]$treeMap))
  CutValue <- nodeNum <- vector("integer", length(forest$trees[[numTree]]$treeMap))
  nodeClass <- nodeNum <- vector("integer", length(forest$trees[[numTree]]$treeMap))
  CutFeature <- vector("list", length(forest$trees[[numTree]]$treeMap))

  currentNode <- 0
  for (z in forest$trees[[numTree]]$treeMap) {
    currentNode <- currentNode + 1
    nodeNum[currentNode] <- currentNode
    tmpPosition <- z
    if (tmpPosition > 0) {
      LC[currentNode] <- forest$trees[[numTree]]$treeMap[currentNode] * 2
      RC[currentNode] <- forest$trees[[numTree]]$treeMap[currentNode] * 2 + 1

      CutValue[currentNode] <- forest$trees[[numTree]]$CutPoint[forest$trees[[numTree]]$treeMap[currentNode]]
      firstFeaturePos <- forest$trees[[numTree]]$matAindex[tmpPosition] + 1
      secondFeaturePos <- forest$trees[[numTree]]$matAindex[tmpPosition + 1]
      CutFeature[[currentNode]] <- forest$trees[[numTree]]$matAstore[firstFeaturePos:secondFeaturePos]
    } else {
      tmpPosition <- -1 * tmpPosition
      nodeClass[currentNode] <- which.max(forest$trees[[numTree]]$ClassProb[tmpPosition, ])
      CutValue[currentNode] <- NA
      CutFeature[[currentNode]] <- NA
    }
  }

  dfRet <- data.frame(nodeNum, LC, RC, CutValue, nodeClass)
  if (nrow(dfRet) != length(CutFeature)) {
    print
    warning("not enough cut features")
    return()
  }
  # print(data.frame(nodeNum, LC, RC, CutValue, nodeClass))
  dfRet$CutFeature <- CutFeature
  print(dfRet)
}
