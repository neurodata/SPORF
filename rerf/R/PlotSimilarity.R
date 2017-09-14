#' Plot similarity
#'
#' Plots a heatmap of the pairwise similarity of points.  
#'
#' @return sqplot
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#'
#' @export
#' @importFrom ggplot2 reshape2
#'



PlotSimilarity <- function(mtx, title="",xlabel="ROI", ylabel="ROI", legend.name="metric", legend.show=TRUE, itype="sq",
                                  font.size=12, rem_diag=FALSE, include_diag=FALSE, limits=c(0, 1),
                           xticks = NULL, yticks = NULL, xticklabels = NULL, yticklabels = NULL) {
  if (itype == "ts") {
    mtx <- abs(cor(mtx))  # if a timeseries is passed in, correlate the features first
  }
  if (!include_diag) {
    diag(mtx) <- 0
  }
  dm <- melt(mtx)
  colnames(dm) <- c("x", "y", "value")
  jet.colors <- colorRampPalette(c("#00007F", "blue", "#007FFF", "cyan", "#7FFF7F", "yellow", "#FF7F00", "red", "#7F0000"))
  sqplot <- ggplot(dm, aes(x=x, y=y, fill=value)) +
    geom_tile() +
    scale_fill_gradientn(colours=jet.colors(7), name=legend.name, limits=limits) +
    xlab(xlabel) +
    ylab(ylabel) +
    ggtitle(title)
  if (legend.show) {
    sqplot <- sqplot +
      theme(text=element_text(size=font.size))
  } else {
    sqplot <- sqplot +
      theme(text=element_text(size=font.size, legend.position="none"))
  }
  if (!is.null(xticks)) {
    if (!is.null(xticklabels)) {
      sqplot <- sqplot + scale_x_continuous(breaks=xticks, labels=xticklabels)
    } else {
      sqplot <- sqplot + scale_x_continuous(breaks=xticks)
    }
  }
  if (!is.null(yticks)) {
    if (!is.null(yticklabels)) {
      sqplot <- sqplot + scale_y_continuous(breaks=yticks, labels=yticklabels)
    } else {
    sqplot <- sqplot + scale_y_continuous(breaks=yticks)
    }
  }
  return(sqplot)
}