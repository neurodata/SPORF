

require(ggplot2)
dat <- read.csv("s-rerf_runs.csv")

ggplot(dat, aes(x = iterate, y = Lhat, group = classifier, color = classifier)) + geom_line()
