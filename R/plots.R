#' @param X prediction data points. an n by d numeric matrix. The rows correspond to observations and columns correspond to features.
#' @param Y truth labels. n length vector
#' @param forest a trained forest
#' @param N integer that determines the size of grid. Higher means finer grid, and more detailed plot
#' @param log string. Axis to change to log scale. Either 'x', 'y' or 'xy'.
#' @param points Whether to plot X
#' @param contours Whether to plot contour lines
#' @param legend Whether to plot legend

PlotBoundary.2Class <- function(X, Y=NULL, forest, N=200L, log='', points=TRUE, contours=TRUE, legend=TRUE) {
  collist.diverging <- c('#c51b7d','#e9a3c9','#fde0ef','#e6f5d0','#a1d76a','#4d9221')

  pred = Predict(X, forest)
  num.classes <- length(levels(pred))

  grid = expand.grid(
    X.1 = seq(min(X[,1]), max(X[,1]), length.out = N),
    X.2 = seq(min(X[,2]), max(X[,2]), length.out = N)
  )

  names(grid) <- names(X)

  pred.grid <- Predict(grid, forest, output.scores = TRUE)
  pred.grid <- matrix(pred.grid, ncol = N, nrow = N)

  x.plot <- unique(grid[, 1])
  y.plot <- unique(grid[, 2])

  image(
    x.plot,
    y.plot,
    pred.grid,
    useRaster = TRUE,
    col = colorRampPalette(collist.diverging)(N),
    xlab = names(grid)[1],
    ylab = names(grid)[2],
    log = log
    #main = "RerF"
  )

  if (points) {
    if (!is.null(Y)) {
      pch = c(16, 17)[as.numeric(pred != Y) + 1]
    } else {
      pch = c(16,17)[as.numeric(pred)]
    }
    points(
      X,
      pch = pch,
      col = adjustcolor("black", alpha = 0.5)
    )
  }

  if (contours) {
    contour(
      x.plot,
      y.plot,
      pred.grid,
      add = TRUE,
      levels = c(.75),
      labcex = 1,
      lwd = 1.75
    )
  }

  if (legend) {
    if (!is.null(Y)) {
      leg = c("correct", "incorrect")
    } else {
      leg = levels(pred)
    }
    legend("bottomright", xpd=NA, legend = leg, pch = c(16,17), bg = "white")
  }
}
