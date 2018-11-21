#' A subset of the MNIST dataset for handwritten digit classification
#'
#' A dataset consiting of 10 percent of the MNIST training set and the
#' full test set
#'
#' @docType data
#'
#' @usage data(mnist)
#'
#' @format A list with four items: Xtrain is a training set matrix with 6000 rows (samples) and 784 columns (features),
#' Xtrain is an integer array of corresponding training class labels, Xtest is a test set matrix of 10000 rows and 784
#' columns, and Ytest is the corresponding class labels. Rows in Xtrain and Xtest correspond to different images of digits,
#' and columns correspond to the pixel intensities in each image, obtained by flattening the image pixels in column-major ordering.
#'
#' @keywords datasets
#'
#' @references Y. LeCun, L. Bottou, Y. Bengio, and P. Haffner. "Gradient-based learning applied to document recognition."
#' Proceedings of the IEEE, 86(11):2278-2324, November 1998.
#'
#' @source \href{http://yann.lecun.com/exdb/mnist/}{MNIST}
#'
#' @examples
#' data(mnist)
"mnist"
