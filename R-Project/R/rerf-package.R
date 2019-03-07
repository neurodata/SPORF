#' rerf-package
#'
#' The rerf package provides an R implementation of the randomer forest
#' algorithm developed by Tomita et al.(2016) <arXiv:1506.03410v2>,
#' along with variants such as Structured Randomer Forest (S-RerF) and
#' Similarity Randomer Forest (SmerF).
#' 
#' 
#' @section The main function \code{RerF}:
#' The \code{RerF} function is the primary access point to the
#' algorithm. Variants of the algorithm can be controlled by specifying
#' the appropriate set of parameters, explained in the sections below.
#'
#' @section RandMat*:
#' The set of \code{RandMat*} functions specify the distribution of
#' projection weights used to split the data at each node.
#'
#' \describe{
#'   \item{RandMatBinary}{samples projection weights from \eqn{{-1,1}}.}
#'   \item{RandMatContinuous}{samples projection weights from \eqn{N(0,1)}}
#'   \item{RandMatPoisson}{samples projection weights from a \eqn{N(0,1)} with sparsity \eqn{~ Poisson(\lambda)}}
#' } 
#'
#' @section RerF variants:
#' \describe{
#'   \item{RF}{Use \code{FUN = RandMatRF} in the call to \code{RerF}}
#'   \item{RerF}{Use \code{FUN = RandMatBinary} in the call to \code{RerF}}
#'   \item{S-RerF}{Use \code{FUN = RandMatImagePatch} in the call to \code{RerF}}
#'   \item{SmerF}{Set \code{task = "similarity"} in the call to \code{RerF}}
#' } 
#' 
#' 
"_PACKAGE"
