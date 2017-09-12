#' Create a ????? Random Matrix 
#'
#' Create a random????? matrix using given params.
#'
#' @param n ?????
#'
#' @return random.matrix ?????
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#' 
#'
#' @importFrom parallel nextRNGStream
#'

skip.streams <-
function(n) {
  x <- .Random.seed
  for (i in seq_len(n))
    x <- parallel::nextRNGStream(x)
  assign('.Random.seed', x, pos=.GlobalEnv)
}
