#' RerF Tree Printer
#'
#' Prints the layout of a specified tree.
#'
#' @param forest a rerf forest structure.
#' @param numTree the tree number to print. (numTree=1)
#' @param pretty boolean if TRUE the column of cut features are formatted nicely
#' for viewing. (FALSE)
#'
#' @return a data.frame with the following information about the tree:
#' \itemize{
#'  \item \code{nodeNum} The node number
#'  \item \code{LC} The id of the left child of the node
#'  \item \code{RC} The id of the right child of the node
#'  \item \code{CutValue} The cut value of non-terminal nodes, otherwise NA.
#'  \item \code{nodeClass} The class vote of a terminal node when used for classification/prediction.
#'  \item \code{CutFeatures} a list of ordered pairs \eqn{(d, w)}, where
#'  \code{d} is the original feature and \code{w} is the corresponding
#'  weight.
#' }
#'
#' @export
#'
#' @examples
#' ### Train RerF on numeric data ###
#' library(rerf)
#' numTree <- 1
#' forest <- RerF(as.matrix(iris[, 1:4]), iris[, 5], num.core = 1L)
#' forest.rmc <- RerF(as.matrix(iris[, 1:4]), iris[, 5], num.core = 1L, RandMatContinuous)
#' (out <- PrintTree(forest, numTree))
#' (out.rmc <- PrintTree(forest.rmc, numTree))
#'


PrintTree <- function(forest, numTree = 1, pretty = FALSE) {

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
  LC <- vector("integer", length(forest$trees[[numTree]]$treeMap))
  RC <- vector("integer", length(forest$trees[[numTree]]$treeMap))
  CutValue <- vector("integer", length(forest$trees[[numTree]]$treeMap))
  nodeClass <- vector("integer", length(forest$trees[[numTree]]$treeMap))
  classProb <- vector("numeric", length(forest$trees[[numTree]]$treeMap))
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
      classProb[currentNode] <- forest$trees[[numTree]]$ClassProb[tmpPosition, which.max(forest$trees[[numTree]]$ClassProb[tmpPosition, ])]
      CutValue[currentNode] <- NA
      CutFeature[[currentNode]] <- NA
    }
  }

  ## Set zero values to NA to avoid confusion
  nodeClass[which(nodeClass == 0)] <- NA
  LC[which(LC == 0)] <- NA
  RC[which(RC == 0)] <- NA

  dfRet <- data.frame(nodeNum, LC, RC, CutValue, nodeClass, classProb)
  if (nrow(dfRet) != length(CutFeature)) {
    print
    warning("not enough cut features")
    return()
  }

  # print(data.frame(nodeNum, LC, RC, CutValue, nodeClass))
  ## The below code formats the CutFeatures nicely for visualization
  prettyCut <- as.list(rep(NA, length = length(CutFeature)))
  for (ci in 1:length(CutFeature)) {
    if (!any(is.na(CutFeature[[ci]]))) {
      Cut <- CutFeature[[ci]]
      tmp <- list()
      for (i in seq(1, length(Cut), by = 2)) {
        tmp <- c(tmp, list(Cut[c(i, i + 1)]))
      }
      prettyCut[[ci]] <- Reduce(
        f = function(x, y) paste(x, y, sep = ", "),
        lapply(tmp, function(x) paste0("(", x[1], ",", x[2], ")"))
      )
    }
  }
  dfRet$CutFeature <- CutFeature
  dfRet$prettyPrint <- prettyCut

  if (!pretty) {
    return(dfRet[, -which(colnames(dfRet) == "prettyPrint")])
  } else {
    return(dfRet[, -which(colnames(dfRet) == "CutFeature")])
  }
}
