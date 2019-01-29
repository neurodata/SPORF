#
args = commandArgs(trailingOnly=TRUE)

if (length(args) <= 1) {
  stop("The output file, params file, and line number must be supplied.\n", call.=FALSE)
} else if (length(args) == 3) {
  outputFile <- as.character(args[1])
  paramsFile <- as.character(args[2])
  lineNum <- as.numeric(args[3])
}

if(any(is.null(c(outputFile, paramsFile, lineNum)))) {
  stop("Failed to specify the correct parameters.")
}

source('RunDataSetWithOptions.R')
require(rerf)
require(slb)

params <- read.csv(paramsFile)[lineNum, ]
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
               'commitSHA1' = op$commitSHA,
               out)

sprintf("Saving dataset %s", op$dataset)

saveName <- sprintf("%s/%s_%s_%s.RData", outputFile, op$dataset, op$RandMats, op$Scale01)
save(storeData, file = saveName)

sprintf("Finished dataset %s", op$dataset)
