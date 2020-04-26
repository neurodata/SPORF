#' Predict responses on out-of-bag observations using a single tree.
#'
#' This is the base function called by OOBPredict.
#'
#' @param X an n sample by d feature matrix (preferable) or data frame which was used to train the provided forest.
#' @param tree a tree from a forest returned by RerF.
#'
#' @return predictions prediction matrix used by OOBPredict
#'

RunOOBReg <-
    function(X, tree) {
      n.all <- nrow(X)

      # Get OOB samples
      X <- X[tree$ind, , drop = FALSE]

      # Predict OOB samples
      predictions <- RunPredictReg(X, tree)

      # Create a matrix for all of samples
      out <- numeric(n.all)
      out[-tree$ind] <- NA
      out[tree$ind] <- predictions
      return(out)
    }
