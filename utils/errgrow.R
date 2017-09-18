errgrow <-
function(Y,probmat){
  z<-integer(length(Y))
  forestSize <- length(probmat) - 1L
  numClass <- ncol(probmat[[1L]])-2
  probcurrent<- matrix(data = 0, nrow = length(Y), ncol = numClass)
  numWrong<- 0L
  numTotal<- 0L
  for(q in 1L:forestSize){
    for(j in 1L:nrow(probmat[[q]])){
      probcurrent[probmat[[q]][j,1L], ] <- probcurrent[probmat[[q]][j,1L], ]+probmat[[q]][j,3:(2+numClass)]
    }
    numWrong<- 0L
    numTotal<- 0L
    for(m in 1L:length(Y)){
      if(any(probcurrent[m,]!=0)){
        if(order(probcurrent[m,],decreasing=T)[1L]!=Y[m]){
          numWrong <- numWrong+1L
        }
        numTotal<-numTotal+1L
      }
    }
    z[q] <- numWrong/numTotal
  }
  return(z)
}
