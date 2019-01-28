#
args = commandArgs(trailingOnly=TRUE)

if (length(args)==0) {
  stop("At least one argument must be supplied (input file).n", call.=FALSE)
} else if (length(args)==1) {
  # default output file
  lineNum <- as.numeric(args[1])
}


source('RunDataSetWithOptions.R')
require(rerf)
require(slb)

params <- read.csv("params.csv")[lineNum, ]
op <- lapply(params, as.character)

dat <- slb.load.datasets(repositories = params$repository, 
                          task = params$task, 
                          datasets = params$dataset, 
                          clean.ohe = TRUE)

data <- dat[[1]]
## the main thing

train.ind <- 1:nrow(data$X) %in% sample(nrow(data$X), floor(3/4 * nrow(data$X)))

X <- data$X[train.ind, ]
Y <- data$Y[train.ind]
Xtest <- data$X[!train.ind, ]
Ytest <- data$Y[!train.ind]

FUN <- match.fun(as.character(params$RandMats))

scaleAtNode <- params$Scale01


sprintf("Running dataset %s", op$dataset)
out <- RunRerF(X, Y, FUN, paramList = NULL, scaleAtNode = scaleAtNode)


## Prepend some more parameter data
storeData <- c('row'     = lineNum,
               'dataset' = op$dataset, 
               'RandMat' = op$RandMats, 
               'Scale01' = op$Scale01, 
               out)

sprintf("Saving dataset %s", op$dataset)

saveName <- 
  sprintf("%s_%s_%s.RData", op$dataset, op$RandMats, op$Scale01)
save(storeData, file = sprintf("%s_testing.RData", params$dataset))

sprintf("Finished dataset %s", op$dataset)
