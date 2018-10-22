preprocess.classification <- function(X, Y, cat.map, stratify, paramList) {
  forest <- list(trees = NULL, labels = NULL, params = NULL)

  # check if data matrix X has one-of-K encoded categorical features that
  # need to be handled specially using RandMatCat instead of RandMat
  if (!is.null(cat.map)) {
    paramList$catMap <- cat.map
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
  if(stratify){
    Cindex <- vector("list",num.class)
    for (m in 1L:num.class) {
      Cindex[[m]] <- which(Y == m)
    }
  } else {
    Cindex <- NULL
  }

  # address na values.
  if (any(is.na(X)) ) {
    if (exists("na.action")) {
      stats::na.action(X,Y)
    }
    if (any(is.na(X))) {
      warning("NA values exist in data matrix")
    }
  }

  return(list(
    X = X,
    Y = Y,
    classCt = classCt,
    Cindex = Cindex,
    paramList = paramList,
    forest = forest
  ))
}


preprocess.regression <- function(X, Y) {
  forest <- list(trees = NULL, labels = NULL, params = NULL)

  # adjust Y to go from 1 to num.class if needed
  if (is.numeric(Y)) {
    forest$labels <- sort(unique(Y))
    Y <- as.integer(as.factor(Y))
  } else {
    stop("Incompatible data type for regression. Y must be of type numeric.")
  }

  # address na values.
  if (any(is.na(X)) ) {
    if (exists("na.action")) {
      stats::na.action(X,Y)
    }
    if (any(is.na(X))) {
      warning("NA values exist in data matrix")
    }
  }

  return(list(
    X = X,
    Y = Y,
    forest = forest
  ))
}
