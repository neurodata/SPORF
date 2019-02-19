#' @importFrom stats rnorm

RandRot <-
  function(p) {
    # %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    # Samples a p x p uniformly random rotation matrix via QR decomposition
    # of a matrix with elements sampled iid from a standard normal distribution
    # %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    RotMat <- qr.Q(qr(matrix(stats::rnorm(p^2), p, p)))
    return(RotMat)
  }
