#' Compute Feature Importance of a RerF model
#'
#' Computes feature importance of every unique feature used to make a split in the RerF model.
#'
#' @param forest a forest trained using the RerF function with argument store.impurity = TRUE
#' @param num.cores number of cores to use. If num.cores = 0, then 1 less than the number of cores reported by the OS are used. (num.cores = 0)
#' @param type character string specifying which method to use in
#' @param X the n-by-p training matrix
#' @param Y the n-length vector of training responses
#' calculating feature importance.
#' \describe{
#'   \item{'C'}{specifies that unique combinations of features
#'   should be *c*ounted across trees.}
#'   \item{'R'}{feature importance will be calculated as in *R*andomForest.}
#'   \item{'E'}{calculates the unique projections up to *e*quivalence if
#'   the vector of projection weights parametrizes the same line in
#'   \eqn{R^p}.}
#' }
#'
#' @return a list with 3 elements,
#' \describe{
#'   \item{\code{imp}}{The vector of scores/counts, corresponding to each feature.}
#'   \item{\code{features}}{The features/projections used.}
#'   \item{\code{type}}{The code for the method used.}
#'   }
#'
#' @examples
#' library(rerf)
#' num.cores <- 1L
#' forest <- RerF(as.matrix(iris[, 1:4]), iris[[5L]], num.cores = 1L, store.impurity = TRUE)
#'
#' imp.C <- FeatureImportance(forest, num.cores, "C")
#' imp.R <- FeatureImportance(forest, num.cores, "R")
#' imp.E <- FeatureImportance(forest, num.cores, "E")
#'
#' fRF <- RerF(as.matrix(iris[, 1:4]), iris[[5L]],
#'   FUN = RandMatRF, num.cores = 1L, store.impurity = TRUE
#' )
#'
#' fRF.imp <- FeatureImportance(forest = fRF, num.cores = num.cores)
#' @export
#' @importFrom parallel detectCores makeCluster clusterExport parSapply stopCluster
#' @importFrom utils object.size

FeatureImportance <- function(forest, num.cores = 0L, type = NULL, X = NULL, Y = NULL) {

  ## choose method to use for calculating feature importance
  if (is.null(type)) {
    if (identical(forest$params$fun, rerf::RandMatRF)) {
      type <- "R"
    } else if (identical(forest$params$fun, rerf::RandMatBinary)) {
      type <- "E"
    } else {
      type <- "C"
    }
  }

  num.trees <- length(forest$trees)

  # type = "P" specifies that we want to use OOB permutation error instead of gini
  if (!identical(type, "P")) {
    num.splits <- sapply(forest$trees, function(tree) length(tree$CutPoint))

    forest.projections <- vector("list")

    ## Iterate over trees in the forest to save all projections used
    for (t in 1:num.trees) {
      tree.projections <-
        lapply(1:num.splits[t], function(nd) {
          forest$trees[[t]]$matAstore[(forest$trees[[t]]$matAindex[nd] + 1L):forest$trees[[t]]$matAindex[nd + 1L]]
        })

      forest.projections <- c(forest.projections, tree.projections)
    }

    ## Calculate the unique projections used according to the distribution
    ## of weights
    if (identical(type, "C")) {
      message("Message: Computing feature importance as counts of unique feature combinations.\n")
      ## compute the unique combinations of features used in the
      ## projections
      unique.projections <- unique(lapply(forest.projections, getFeatures))

      CompImportanceCaller <- function(tree, ...) {
        RunFeatureImportanceCounts(tree = tree, unique.projections = unique.projections)
      }
      varlist <- c("unique.projections", "RunFeatureImportanceCounts")
    }

    if (identical(type, "R")) {
      message("Message: Computing feature importance for RandMatRF.\n")
      ## Compute the unique projections without the need to account for
      ## 180-degree rotations.
      unique.projections <- unique(forest.projections)

      CompImportanceCaller <- function(tree, ...) {
        RunFeatureImportance(tree = tree, unique.projections = unique.projections)
      }
      varlist <- c("unique.projections", "RunFeatureImportance")
    }

    if (identical(type, "E")) {
      message("Message: Computing feature importance for RandMatBinary.\n")
      ## compute the unique projections properly accounting for
      ## projections that differ by a 180-degree rotation.
      unique.projections <- uniqueByEquivalenceClass(
        forest$params$paramList$p,
        unique(forest.projections)
      )

      CompImportanceCaller <- function(tree, ...) {
        RunFeatureImportanceBinary(
          tree = tree,
          unique.projections = unique.projections
        )
      }
      varlist <- c("unique.projections", "RunFeatureImportanceBinary")
    }



    if (num.cores != 1L) {
      if (num.cores == 0L) {
        # Use all but 1 core if num.cores=0.
        num.cores <- parallel::detectCores() - 1L
      }
      num.cores <- min(num.cores, num.trees)
      gc()
      if ((utils::object.size(forest) > 2e9) |
        .Platform$OS.type == "windows") {
        cl <- parallel::makeCluster(spec = num.cores, type = "PSOCK")
        parallel::clusterExport(cl = cl, varlist = varlist, envir = environment())
        feature.imp <- parallel::parSapply(cl = cl, forest$trees, FUN = CompImportanceCaller)
      } else {
        cl <- parallel::makeCluster(spec = num.cores, type = "FORK")
        feature.imp <- parallel::parSapply(cl = cl, forest$trees, FUN = CompImportanceCaller)
      }

      parallel::stopCluster(cl)
    } else {
      # Use just one core.
      feature.imp <- sapply(forest$trees, FUN = CompImportanceCaller)
    }

    feature.imp <- apply(feature.imp, 1L, sum)
    sort.idx <- order(feature.imp, decreasing = TRUE)
    feature.imp <- feature.imp[sort.idx]
    unique.projections <- unique.projections[sort.idx]

    return(feature.imp <- list(imp = feature.imp, features = unique.projections, type = type))
  } else {
    if (is.null(X) || is.null(Y)) {
      stop("Permutation OOB Error method was specified but training data X and Y were not provided.")
    }

    n <- dim(X)[1L]
    p <- dim(X)[2L]

    # compute baseline Error
    # predictions <- OOBPredict(X, forest, num.cores = num.cores)
    predictions <- Predict(X, forest, OOB = TRUE, num.cores = num.cores)
    if (forest$params$task == "classification") {
      baseline.error <- mean(predictions != Y)
    } else if (forest$params$task == "regression") {
      baseline.error <- mean((predictions - Y)^2)
    }

    # compute error for each permuted feature
    permuted.error <- rep(0, p)
    for (j in seq.int(p)) {
      unshuffled.feature <- X[, j]
      X[, j] <- sample(X[, j])
      # predictions <- OOBPredict(X, forest, num.cores = num.cores)
      predictions <- Predict(X, forest, OOB = TRUE, num.cores = num.cores)
      if (forest$params$task == "classification") {
        permuted.error[j] <- mean(predictions != Y)
      } else if (forest$params$task == "regression") {
        permuted.error[j] <- mean((predictions - Y)^2)
      }
      X[, j] <- unshuffled.feature
    }
    feature.imp <- (permuted.error - baseline.error)/baseline.error
    return(feature.imp)
  }
}
