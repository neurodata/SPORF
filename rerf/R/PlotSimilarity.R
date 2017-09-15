#' Plot similarity
#'
#' Plots a heatmap of the pairwise similarity of points.  
#' 
#' @param sim an n-by-n similarity matrix. The element in the ith row and jth column is the similarity between the ith and jth observation of a set of data.
#' @param title a string specifying the title of the plot.
#' @param xlabel a string specifying the x-axis label.
#' @param ylabel a string specifying the y-axis label.
#' @param legend.name a string specifying the metric of the colorbar.
#' @param legend.show if TRUE then the colorbar is displayed.
#' @param font.size a number specifying the font size of the plot.
#' @param include.diag if TRUE then the diagonal is not zeroed out.
#' @param limits a numeric vector of length two specifying the min and max values corresponding to the color gradient scale.
#' @param xticks a numeric vector specifying the values on the x-axis at which ticks will be placed.
#' @param yticks a numeric vector specifying the values on the y-axis at which ticks will be placed.
#' @param xticklabels a charcater vector the same length as xticks specifying the labels of the xticks.
#' @param yticklabels a charcater vector the same length as yticks specifying the labels of the yticks.
#'
#' @return simplot
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#'
#' @export
#' @importFrom ggplot2 reshape2
#'



PlotSimilarity <- function(sim, title="",xlabel="ROI", ylabel="ROI", legend.name="metric", legend.show=TRUE, font.size=12,
                           include.diag=FALSE, limits=c(0, 1), xticks = NULL, yticks = NULL, xticklabels = NULL, yticklabels = NULL) {
  if (!include.diag) {
    diag(sim) <- 0
  }
  dm <- melt(sim)
  colnames(dm) <- c("x", "y", "value")
  jet.colors <- colorRampPalette(c("#00007F", "blue", "#007FFF", "cyan", "#7FFF7F", "yellow", "#FF7F00", "red", "#7F0000"))
  simplot <- ggplot(dm, aes(x=x, y=y, fill=value)) +
    geom_tile() +
    scale_fill_gradientn(colours=jet.colors(7), name=legend.name, limits=limits) +
    xlab(xlabel) +
    ylab(ylabel) +
    ggtitle(title)
  if (legend.show) {
    simplot <- simplot +
      theme(text=element_text(size=font.size))
  } else {
    simplot <- simplot +
      theme(text=element_text(size=font.size, legend.position="none"))
  }
  if (!is.null(xticks)) {
    if (!is.null(xticklabels)) {
      simplot <- simplot + scale_x_continuous(breaks=xticks, labels=xticklabels)
    } else {
      simplot <- simplot + scale_x_continuous(breaks=xticks)
    }
  }
  if (!is.null(yticks)) {
    if (!is.null(yticklabels)) {
      simplot <- simplot + scale_y_continuous(breaks=yticks, labels=yticklabels)
    } else {
    simplot <- simplot + scale_y_continuous(breaks=yticks)
    }
  }
  return(simplot)
}