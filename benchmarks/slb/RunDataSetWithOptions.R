#' Train a forest and output timings and stats
#'
#' @param X an n by d numeric matrix (preferable) or data frame. The rows correspond to observations and columns correspond to features.
#' @param Y an n length vector of class labels.  Class labels must be integer or numeric and be within the range 1 to the number of classes.
#' @param FUN a function that creates the random projection matrix. If NULL and cat.map is NULL, then RandMat is used. If NULL and cat.map is not NULL, then RandMatCat is used, which adjusts the sampling of features when categorical features have been one-of-K encoded. If a custom function is to be used, then it must return a matrix in sparse representation, in which each nonzero is an array of the form (row.index, column.index, value). See RandMat or RandMatCat for details.
#' @param paramList parameters in a named list to be used by FUN. If left unchanged,
#' default values will be populated, see \code{\link[rerf]{defaults}} for details.
#'
#' @return forest
#'

RunRerF <- function(X, Y, FUN, paramList, scaleAtNode = FALSE) {

  training.time <- 
    system.time({
      forest <- RerF(X, Y, FUN = FUN, store.oob = TRUE, store.impurity = TRUE, scaleAtNode = scaleAtNode)
    })

  train.prediction.time <- 
    system.time({
      training.predictions <- Predict(X, forest)
      })

  training.error <- mean(training.predictions != Y)

  oob.predict.time <- 
    system.time({
      OOB.predictions <- OOBPredict(X, forest)
    })

  OOB.error <- mean(OOB.predictions != Y)

  test.prediction.time <- 
    system.time({
      testing.predictions <- Predict(Xtest, forest)
      })

  testing.error <- mean(testing.predictions != Ytest)

  imp.time <- 
    system.time({
      feature.importance.R <- FeatureImportance(forest, type = "R")
    })


  out <- list(
              ## Data
              forest = forest,
              FUN = FUN,
              paramList = paramList,
              ## Errors
              testing.error = testing.error,
              training.error = training.error,
              OOB.error = OOB.error,
              ## Predictions
              testing.predictions = testing.predictions,
              training.predictions = training.predictions,
              OOB.predictions = OOB.predictions,
              ## Timings
              training.time = training.time,
              train.prediction.time = train.prediction.time,
              test.prediction.time = test.prediction.time,
              oob.predict.time = oob.predict.time,
              imp.time = imp.time
             )
}



#   Time:
##  Working status:
### Comments:
####Soli Deo Gloria
