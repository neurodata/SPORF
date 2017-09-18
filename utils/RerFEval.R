#' Rerf Evaluate 
#'
#' Evaluate Rerf models for several values of mat.options[[2]] and mat.options[[4]]
#'
#' @param Xtrain an n sample by d feature matrix (preferable) or data frame that will be used to train a forest.
#' @param Ytrain an n length vector of class labels
#' @param Xtest an n sample by d feature matrix (preferable) or data frame that will be used to test a forest.
#' @param Ytest an n length vector of class labels
#' @param params a list of parameters used in the various rerf functions
#' @param store.predictions ????? (store.predictions=FALSE)
#'
#' @return something ?????
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#' 
#' @importFrom AUC auc roc
#' @importFrom compiler setCompilerOptions cmpfun
#'

RerFEval <-
    function(Xtrain, Ytrain, Xtest, Ytest, params = list(trees = 500L, randomMatrix = "binary", d = round(sqrt(ncol(Xtrain))), sparsity = 1/ncol(Xtrain), rotate = F, rank.transform = F, MinParent = 2L, MaxDepth = "inf", bagging = 1/exp(1), COOB = T, CNS = F, replacement = T, stratify = T, num.cores = 1L, seed = 1L), store.predictions = F) {

        p <- ncol(Xtrain)
        nClasses <- length(unique(Ytrain))

        params.names <- names(params)

        if (!("trees" %in% params.names)) {
            params$trees <- 500L
        }

        if (!("randomMatrix" %in% params.names)) {
            params$randomMatrix <- "binary"
        }

        if (!("d" %in% params.names)) {
            params$d <- round(sqrt(p))
        }

        if (!("sparsity" %in% params.names)) {
            if (params$randomMatrix == "binary" || params$randomMatrix == "continuous") {
                params$sparsity <- 1/p
            } else if (params$randomMatrix == "frc" || params$randomMatrix == "frcn") {
                params$sparsity <- 2
            } else if (params$randomMatrix == "poisson") {
                params$sparsity <- 1
            } else if (params$randomMatrix == "rf") {
                params$sparsity <- 1
            }
        }

        if (!("rotate" %in% params.names)) {
            params$rotate <- F
        }

        if (!("rank.transform" %in% params.names)) {
            params$rank.transform <- F
        }

        if (!("MinParent" %in% params.names)) {
            params$MinParent <- 2L
        }

        if (!("MaxDepth" %in% params.names)) {
            params$MaxDepth <- "inf"
        }

        if (!("bagging" %in% params.names)) {
            params$bagging <- 1/exp(1)
        }

        if (!("COOB" %in% params.names)) {
            params$COOB <- T
        }

        if (!("CNS" %in% params.names)) {
            params$CNS <- F
        }

        if (!("replacement" %in% params.names)) {
            params$replacement <- T
        }

        if (!("stratify" %in% params.names)) {
            params$stratify <- T
        }

        if (!("num.cores" %in% params.names)) {
            params$num.cores <- 1L
        }

        if (!("seed" %in% params.names)) {
            params$seed <- 1L
        }
        set.seed(params$seed)

        # compile before the actual run so that training time of first iteration is consistent with the rest
        compiler::setCompilerOptions("optimize"=3)
        comp_tree <- compiler::cmpfun(BuildTree)
        comp_errOOB <- compiler::cmpfun(runerrOOB)
        comp_predict <- compiler::cmpfun(runpredict)



        if (params$randomMatrix == "binary" || params$randomMatrix == "continuous" || params$randomMatrix == "poisson" ||
            params$randomMatrix == "frc" || params$randomMatrix == "frcn") {
            nforest <- length(params$d)*length(params$sparsity)
            trainTime <- vector(mode = "numeric", length = nforest)
            oobTime <- vector(mode = "numeric", length = nforest)
            testTime <- vector(mode = "numeric", length = nforest)
            testError <- vector(mode = "numeric", length = nforest)
            oobError <- vector(mode = "numeric", length = nforest)
            oobAUC <- vector(mode = "numeric", length = nforest)
            treeStrength <- vector(mode = "numeric", length = nforest)
            treeCorrelation <- vector(mode = "numeric", length = nforest)
            numNodes <- vector(mode = "numeric", length = nforest)
            if (store.predictions) {
                Yhat <- matrix(0, nrow = Xtest, ncol = nforest)
            }
            for (i in 1:length(params$sparsity)) {
                for (j in 1:length(params$d)) {
                    options <- list(p, params$d[j], params$randomMatrix, params$sparsity[i])
                    forest.idx <- (i - 1)*length(params$d) + j

                    print(paste("Evaluating forest ", as.character(forest.idx), " of ", as.character(nforest), sep = ""))

                    # train
                    print("training")
                    start.time <- proc.time()
                    forest <- rerf(Xtrain, Ytrain, trees = params$trees, FUN = RandMat, options = options, rank.transform = params$rank.transform,
                                   MinParent = params$MinParent, MaxDepth = params$MaxDepth, bagging = params$bagging, COOB = params$COOB,
                                   CNS = params$CNS, replacement = params$replacement, stratify = params$stratify, num.cores = params$num.cores, seed = params$seed)
                    trainTime[forest.idx] <- (proc.time() - start.time)[[3L]]
                    print("training complete")
                    print(paste("elapsed time: ", trainTime[forest.idx], sep = ""))

                    # compute out-of-bag metrics
                    print("computing out-of-bag predictions")
                    start.time <- proc.time()
                    oobScores <- OOBPredict(Xtrain, forest, num.cores = params$num.cores, rank.transform = params$rank.transform)
                    oobTime[forest.idx] <- (proc.time() - start.time)[[3L]]
                    print("out-of-bag predictions complete")
                    print(paste("elapsed time: ", oobTime[forest.idx], sep = ""))
                    oobError[forest.idx] <- mean(max.col(oobScores) != Ytrain)
                    if (nClasses > 2L) {
                        Ybin <- as.factor(as.vector(dummy(Ytrain)))
                        oobAUC[forest.idx] <- AUC::auc(roc(as.vector(oobScores), Ybin))
                    } else {
                        # Ytrain starts from 1, but here we need it to start from 0
                        oobAUC[forest.idx] <- AUC::auc(roc(oobScores[, 2L], as.factor(Ytrain - 1L)))
                    }

                    numNodes[forest.idx] <- mean(sapply(forest, FUN = function(tree) length(tree$treeMap)))

                    # make predictions on test set
                    print("computing predictions on test set")
                    start.time <- proc.time()
                    testScores <- predict(Xtest, forest, num.cores = params$num.cores, rank.transform = params$rank.transform, Xtrain = Xtrain)
                    testTime[forest.idx] <- (proc.time() - start.time)[[3L]]
                    print("test set predictions complete")
                    print(paste("elapsed time: ", testTime[forest.idx], sep = ""))

                    # compute error on test set
                    if (store.predictions) {
                        Yhat[, forest.idx] <- max.col(testScores)
                        testError[forest.idx] <- mean(Yhat[, forest.idx] != Ytest)
                    } else {
                        Yhat <- max.col(testScores)
                        testError[forest.idx] <- mean(Yhat != Ytest)
                    }

                    # compute strength and correlation
                    preds <- predict(Xtest, forest, num.cores = params$num.cores, rank.transform = params$rank.transform, Xtrain = Xtrain, out.mode = "individual")
                    sc <- strcorr(preds, Ytest, nClasses)
                    treeStrength[forest.idx] <- sc$s
                    treeCorrelation[forest.idx] <- sc$rho

                    # # save forest models
                    # save(forest, file = fileName)
                }
            }
        } else {
            params$d <- params$d[params$d <= p]
            nforest <- length(params$d)
            trainTime <- vector(mode = "numeric", length = nforest)
            oobTime <- vector(mode = "numeric", length = nforest)
            testTime <- vector(mode = "numeric", length = nforest)
            testError <- vector(mode = "numeric", length = nforest)
            oobError <- vector(mode = "numeric", length = nforest)
            oobAUC <- vector(mode = "numeric", length = nforest)
            treeStrength <- vector(mode = "numeric", length = nforest)
            treeCorrelation <- vector(mode = "numeric", length = nforest)
            numNodes <- vector(mode = "numeric", length = nforest)
            if (store.predictions) {
                Yhat <- matrix(0, nrow = Xtest, ncol = nforest)
            }
            for (forest.idx in 1:nforest) {
                options <- list(p, params$d[forest.idx], params$randomMatrix, NULL)

                print(paste("Evaluating forest ", as.character(forest.idx), " of ", as.character(nforest), sep = ""))

                # train
                print("training")
                start.time <- proc.time()
                forest <- rerf(Xtrain, Ytrain, trees = params$trees, FUN = RandMat, options = options, rank.transform = params$rank.transform,
                               MinParent = params$MinParent, MaxDepth = params$MaxDepth, bagging = params$bagging, COOB = params$COOB,
                               CNS = params$CNS, replacement = params$replacement, stratify = params$stratify, num.cores = params$num.cores, seed = params$seed)
                trainTime[forest.idx] <- (proc.time() - start.time)[[3L]]
                print("training complete")
                print(paste("elapsed time: ", trainTime[forest.idx], sep = ""))

                # compute out-of-bag metrics
                print("computing out-of-bag predictions")
                start.time <- proc.time()
                oobScores <- OOBPredict(Xtrain, forest, num.cores = params$num.cores, rank.transform = params$rank.transform)
                oobTime[forest.idx] <- (proc.time() - start.time)[[3L]]
                print("out-of-bag predictions complete")
                print(paste("elapsed time: ", oobTime[forest.idx], sep = ""))
                oobError[forest.idx] <- mean(max.col(oobScores) != Ytrain)
                if (nClasses > 2L) {
                    Ybin <- as.factor(as.vector(dummies::dummy(Ytrain)))
                    oobAUC[forest.idx] <- AUC::auc(AUC::roc(as.vector(oobScores), Ybin))
                } else {
                    # Ytrain starts from 1, but here we need it to start from 0
                    oobAUC[forest.idx] <- AUC::auc(AUC::roc(oobScores[, 2L], as.factor(Ytrain - 1L)))
                }

                numNodes[forest.idx] <- mean(sapply(forest, FUN = function(tree) length(tree$treeMap)))

                # make predictions on test set
                print("computing predictions on test set")
                start.time <- proc.time()
                testScores <- predict(Xtest, forest, num.cores = params$num.cores, rank.transform = params$rank.transform, Xtrain)
                testTime[forest.idx] <- (proc.time() - start.time)[[3L]]
                print("test set predictions complete")
                print(paste("elapsed time: ", testTime[forest.idx], sep = ""))

                # compute error on test set
                if (store.predictions) {
                    Yhat[, forest.idx] <- max.col(testScores)
                    testError[forest.idx] <- mean(Yhat[, forest.idx] != Ytest)
                } else {
                    Yhat <- max.col(testScores)
                    testError[forest.idx] <- mean(Yhat != Ytest)
                }

                # compute strength and correlation
                preds <- predict(Xtest, forest, num.cores = params$num.cores, rank.transform = params$rank.transform, Xtrain = Xtrain, out.mode = "individual")
                sc <- strcorr(preds, Ytest, nClasses)
                treeStrength[forest.idx] <- sc$s
                treeCorrelation[forest.idx] <- sc$rho
            }
        }

        # select best model
        minError.idx <- which(oobError == min(oobError))
        if (length(minError.idx) > 1L) {
            maxAUC.idx <- which(oobAUC[minError.idx] == max(oobAUC[minError.idx]))
            if (length(maxAUC.idx) > 1L) {
                maxAUC.idx <- sample(maxAUC.idx, 1L)
            }
            best.idx <- minError.idx[maxAUC.idx]  
        } else {
            best.idx <- minError.idx
        }

        if (store.predictions) {
            return(list(Yhat = Yhat[, best.idx], testError = testError, trainTime = trainTime, oobTime = oobTime, testTime = testTime, oobError = oobError, oobAUC = oobAUC, treeStrength = treeStrength, treeCorrelation = treeCorrelation, numNodes = numNodes, best.idx = best.idx, params = params))
        } else {
            return(list(testError = testError, trainTime = trainTime, oobTime = oobTime, testTime = testTime, oobError = oobError, oobAUC = oobAUC, treeStrength = treeStrength, treeCorrelation = treeCorrelation, numNodes = numNodes, best.idx = best.idx, params = params))
        }
    }
