groupidx <-
function(Y) {
  n <- length(Y)
  class.start.idx <- vector("integer", Y[n])
  y <- 1L
  class.start.idx[1L] <- 1L
  for (i in 2:n) {
    if (Y[i] != y) {
      y <- Y[i]
      class.start.idx[y] <- i
      if (y == Y[n]) {
        break
      }
    }
  }
  return(class.start.idx)
}
