rerf <-
function(X, Y, MinParent=6L, trees=100L, MaxDepth=0L, bagging = .2, replacement=TRUE, stratify=FALSE, FUN=makeA, options=c(ncol(X), round(ncol(X)^.5),1L, 1/ncol(X)), rank.transform = FALSE, COOB=FALSE, CNS=FALSE, Progress=FALSE, rotate = F, NumCores=0L, seed = 1L){
  
  #keep from making copies of X
  
  if (!is.matrix(X)) {
    X <- as.matrix(X)
  }
  if (rank.transform) {
    X <- rank.matrix(X)
  }
  if(!is.integer(Y)){
    Y <- as.integer(Y)
  }
  uY<-length(unique(Y))
  classCt <- tabulate(Y,uY)
  for(q in 2:uY){
    classCt[q] <- classCt[q]+classCt[q-1]
  }
  if(stratify){
    Cindex<-vector("list",uY)
    for(m in 1L:uY){
      Cindex[[m]]<-which(Y==m)
    }
  }else{
    Cindex<-NULL
  }
  
  if (!require(compiler)){
    cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to create the forest.\n")
    comp_tree <<- build.tree
  }
  if(!exists("comp_tree")){
    setCompilerOptions("optimize"=3)
    comp_tree <<- cmpfun(build.tree)
  }
  
  if (NumCores!=1L){
    if(require(parallel)){
      RNGkind("L'Ecuyer-CMRG")
      set.seed(seed)
      mc.reset.stream()
      if(NumCores==0){
        #Use all but 1 core if NumCores=0.
        NumCores=detectCores()-1L
      }
      NumCores=min(NumCores,trees)
      mcrun<- function(...) comp_tree(X, Y, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB=COOB, CNS=CNS, Progress=Progress, rotate)
      gc()
      forest <- mclapply(1:trees, mcrun, mc.cores = NumCores, mc.set.seed=TRUE)
    }else{
      #Parallel package not available.
      cat("Package 'parallel' not available.\nExecution will continue without parallelization.\nThis will increase the time required to create the forest\n")
      if (!require(compiler)) {
        cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to create the forest.\n")
        comp_forest <<- function(X, Y, trees, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB, CNS, Progress, rotate, seed) {
          set.seed(seed)
          return(lapply(1:trees, comp_tree(X, Y, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB=COOB, CNS=CNS, Progress=Progress, rotate=rotate)))
        }
      }
      if (!exists("comp_forest")) {
        build.forest <<- function(X, Y, trees, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB, CNS, Progress, rotate, seed) {
          set.seed(seed)
          forest <- vector("list", trees)
          for (t in 1:trees) {
            forest[[t]] <- comp_tree(X, Y, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB=COOB, CNS=CNS, Progress=Progress, rotate=rotate)
          }
          return(forest)
        }
        comp_forest <<- cmpfun(build.forest)
      }
      forest <- comp_forest(X, Y, trees, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB, CNS, Progress, rotate, seed)
    }
  }else{
    #Use just one core.
    if (!require(compiler)) {
      cat("You do not have the 'compiler' package.\nExecution will continue without compilation.\nThis will increase the time required to create the forest.\n")
      comp_forest <<- function(X, Y, trees, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB, CNS, Progress, rotate, seed) {
        set.seed(seed)
        return(lapply(1:trees, comp_tree(X, Y, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB=COOB, CNS=CNS, Progress=Progress, rotate=rotate)))
      }
    }
    if (!exists("comp_forest")) {
      build.forest <- function(X, Y, trees, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB, CNS, Progress, rotate, seed) {
        set.seed(seed)
        forest <- vector("list", trees)
        for (t in 1:trees) {
          forest[[t]] <- comp_tree(X, Y, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB=COOB, CNS=CNS, Progress=Progress, rotate=rotate)
        }
        return(forest)
      }
      comp_forest <<- cmpfun(build.forest)
    }
    forest <- comp_forest(X, Y, trees, MinParent, MaxDepth, bagging, replacement, stratify, Cindex, classCt, FUN, options, COOB, CNS, Progress, rotate, seed)
  }
  if(Progress){
    cat("\n\n")
  }
  
  return(forest)
}
