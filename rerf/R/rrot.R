rrot <-
function(p) {
  #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  # Samples a p x p uniformly random rotation matrix via QR decomposition
  # of a matrix with elements sampled iid from a standard normal distribution
  #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  return(qr.Q(qr(matrix(rnorm(p^2), p, p))))
}
