% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/BuildTree.R
\name{BuildTree}
\alias{BuildTree}
\title{RerF Tree Generator}
\usage{
BuildTree(X, Y, FUN, paramList, min.parent, max.depth, bagging,
  replacement, stratify, class.ind, class.ct, store.oob, store.impurity,
  progress, rotate)
}
\arguments{
\item{X}{an n by d numeric matrix (preferable) or data frame. The rows correspond to observations and columns correspond to features.}

\item{Y}{an n length vector of class labels.  Class labels must be integer or numeric and be within the range 1 to the number of classes.}

\item{FUN}{a function that creates the random projection matrix.}

\item{paramList}{parameters in a named list to be used by FUN. If left unchanged,
default values will be populated, see \code{\link[rerf]{defaults}} for details.}

\item{min.parent}{the minimum splittable node size.  A node size < min.parent will be a leaf node. (min.parent = 6)}

\item{max.depth}{the longest allowable distance from the root of a tree to a leaf node (i.e. the maximum allowed height for a tree).  If max.depth=0, the tree will be allowed to grow without bound.}

\item{bagging}{a non-zero value means a random sample of X will be used during tree creation.  If replacement = FALSE the bagging value determines the percentage of samples to leave out-of-bag.  If replacement = TRUE the non-zero bagging value is ignored.}

\item{replacement}{if TRUE then n samples are chosen, with replacement, from X.}

\item{stratify}{if TRUE then class sample proportions are maintained during the random sampling.  Ignored if replacement = FALSE.}

\item{class.ind}{a vector of lists.  Each list holds the indexes of its respective class (e.g. list 1 contains the index of each class 1 sample).}

\item{class.ct}{a cumulative sum of class counts.}

\item{store.oob}{if TRUE then the samples omitted during the creation of a tree are stored as part of the tree.  This is required to run OOBPredict().}

\item{store.impurity}{if TRUE then the reduction in Gini impurity is stored for every split. This is required to run FeatureImportance().}

\item{progress}{if true a pipe is printed after each tree is created.  This is useful for large datasets.}

\item{rotate}{if TRUE then the data matrix X is uniformly randomly rotated.}
}
\value{
Tree
}
\description{
Creates a single decision tree based on an input matrix and class vector.  This is the function used by rerf to generate trees.
}
\examples{

x <- iris[, -5]
y <- as.numeric(iris[, 5])
# BuildTree(x, y, RandMatBinary, p = 4, d = 4, rho = 0.25, prob = 0.5)
}
