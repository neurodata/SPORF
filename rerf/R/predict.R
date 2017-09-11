#' Predict Sample Classification
#'
#' Predicts the classification of samples using a trained forest.
#'
#' @param X an n sample by d feature matrix (preferable) or data frame. 
#' @param Forest a forest trained using the rerf function.
#' @param NumCores the number of cores to use while training. If NumCores=0 then 1 less than the number of cores reported by the OS are used. (NumCores=0)
#' @param rank.transform ????? (rank.transform=FALSE)
#' @param Xtrain ????? (Xtrain=NULL)
#' @param comp.mode ????? (comp.mode="batch")
#' @param out.mode ????? (out.mode= "aggregate")
#'
#' @return classificationVector an integer vector of length n which specifies the class prediction for each of the n samples.
#'
#' @author James and Tyler, jbrowne6@jhu.edu and
#' 
#' @examples
#' predict(as.matrix(iris[,1:4]), Forest)
#'
#' @export
#' @importFrom compiler setCompilerOptions cmpfun
#' @importFrom parallel detectCores makeCluster clusterExport parLapply stopCluster
#'

predict <-
    function(X, Forest, NumCores=0, rank.transform = F, Xtrain = NULL, comp.mode = "batch", out.mode = "aggregate"){
        if (!is.matrix(X)) {
            X <- as.matrix(X)
        }
        if (rank.transform) {
            if (is.null(Xtrain)) {
                ############ error ############
                stop("The model was trained on rank-transformed data. Xtrain must be provided in order to embed Xtest into the rank space")
            } else {
                X <- rank.interpolate(Xtrain, X)
            }
        }

        if(!requireNamespace(compiler)){
            cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to predict.\n")
            comp_predict <<- runpredict
        }

        if(!exists("comp_predict")){
            compiler::setCompilerOptions("optimize"=3)
            comp_predict <<- compiler::cmpfun(runpredict)
        } 

        comp_predict_caller <- function(tree, ...) comp_predict(X=X, tree=tree, comp.mode = comp.mode)

        f_size <- length(Forest)
        if(NumCores!=1){
            if(requireNamespace(parallel)){
                if(NumCores==0){
                    #Use all but 1 core if NumCores=0.
                    NumCores=parallel::detectCores()-1L
                }
                #Start mclapply with NumCores Cores.
                NumCores <- min(NumCores, f_size)
                gc()
                if ((object.size(Forest) > 2e9) | (object.size(X) > 2e9)) {
                    cl <- parallel::makeCluster(spec = NumCores, type = "PSOCK")
                    parallel::clusterExport(cl = cl, varlist = c("X", "comp.mode", "comp_predict"), envir = environment())
                    Yhats <- parallel::parLapply(cl = cl, Forest, fun = comp_predict_caller)
                } else {
                    cl <- parallel::makeCluster(spec = NumCores, type = "FORK")
                    Yhats <- parallel::parLapply(cl = cl, Forest, fun = comp_predict_caller)
                }
                parallel::stopCluster(cl)
            }else{
                #Parallel package not available.
                cat("Package 'parallel' not available.\nExecution will continue without parallelization.\nThis will increase the time required to predict.\n")
                Yhats <- lapply(Forest, FUN = comp_predict_caller)
            }
        }else{
            #Use just one core.
            Yhats <- lapply(Forest, FUN = comp_predict_caller)
        }

        if (out.mode == "individual") {
            return(matrix(unlist(Yhats), nrow(X), f_size))
        } else {
            num_classes <- ncol(Forest[[1]]$ClassProb)
            scores <- matrix(0,nrow=nrow(X), ncol=num_classes)
            for(m in 1:f_size){
                for(k in 1:nrow(X)){
                    scores[k, Yhats[[m]][k]] <- scores[k, Yhats[[m]][k]] + 1
                }
            }
            scores <- scores/f_size
            return(scores)
        }
    }
