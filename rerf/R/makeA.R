makeA <-
function(options){
  p <- options[[1L]]
  d <- options[[2L]]
  method <- options[[3L]]
  if(method == 1L){
    rho<-options[[4L]]
    nnzs <- round(p*d*rho)
    sparseM <- matrix(0L, nrow=p, ncol=d)
    sparseM[sample(1L:(p*d),nnzs, replace=F)]<-sample(c(1L,-1L),nnzs,replace=T)
  }
  #The below returns a matrix after removing zero columns in sparseM.
  ind<- which(sparseM!=0,arr.ind=TRUE)
  return(cbind(ind,sparseM[ind]))        
}
