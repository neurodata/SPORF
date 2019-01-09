#' Scale data in current Node to [0,1]
#'
#' Scales the data in the current node to live in [0,1] over the columns.
#'
#' @param Xnode an n by d numeric matrix or data frame containing the data assigned to the current node. The rows correspond to observations and columns correspond to features.
#' @param scalingFactors a list with names elements \code{min} and
#' \code{diff} containing vectors of the minima and difference in maxima
#' and minima across columns of Xnode.
#'
#' @return list with the scaled data and scaling factors
#'
#' @export
#'
#' @examples
#' set.seed(12345)
#' X <- as.matrix(iris[, -5])
#' CurrentNode <- 1
#' Assigned2Node <- list(sample(1:nrow(X), 10))
#'
#' sparseM <- RandMatBinary(p = 4, d = 2, sparsity = 0.5, prob = 0.5)
#'
#' lrows  <- which(sparseM[, 2L] == 1)
#' Xnode <- X[Assigned2Node[[CurrentNode]], sparseM[lrows, 2L, drop = FALSE]]
#' scaleDat <- Scale01(X[Assigned2Node[[CurrentNode]], , drop = FALSE])
#'
Scale01 <- function(Xnode, scalingFactors = NULL) {
  Xnode <- cbind(Xnode)

  if (is.null(scalingFactors)) {

    # Find range of current data
    r <- apply(Xnode, 2, range)

    # store scaling factors at this node
    scalingFactors <- list(min = r[1, ], diff = (r[2, ] - r[1, ]))

    # shift data by the minimum
    scaledXnode <- sweep(Xnode, MARGIN = 2, STATS = scalingFactors$min, FUN = "-")

    # scale data if the max - min != 0
    if (any(scalingFactors$diff == 0)) {
      scalingFactors$diff[scalingFactors$diff == 0] <- 1
    }

    scaledXnode <-
      sweep(scaledXnode, 2, STATS = scalingFactors$diff, FUN = "/")

    return(list(scaledXnode = scaledXnode, scalingFactors = scalingFactors))
  } else {
    scaledXnode <-
      sweep(sweep(Xnode, MARGIN = 2, STATS = scalingFactors$min, FUN = "-"),
        MARGIN = 2, STATS = scalingFactors$diff, FUN = "/"
      )

    return(scaledXnode)
  }
}
