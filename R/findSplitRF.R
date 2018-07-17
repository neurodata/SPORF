findSplitRF <- function(x,
                        y,
                        ndSize,
                        I,
                        maxdI,
                        bv,
                        bs,
                        nzidx) {
    duplicateSequence <- as.integer(duplicated(x))
    splitPoints <- which(duplicateSequence == 0)
    if (length(splitPoints) > 1) {
        
        for ( i in 2:length(splitPoints) ) {
            j <- splitPoints[i]-1
            # k <- ifelse(i == length(seqStartIndices), NdSize, seqStartIndices[i+1]-1)
            dI <- I - sum(c(split.mse(y[1:j]), split.mse(y[-(1:j)])))
            if (dI > maxdI) {
                maxdI <- dI
                bv  <- nzidx
                bs  <- (x[j] + x[min(j+1,ndSize)]) / 2
            }
        }
        # for (i in 1:(ndSize-1)) {
        #     dI <- I - sum(c(split.mse(y[1:i]), split.mse(y[-(1:i)])))
        #     if (dI > maxdI) {
        #         maxdI <- dI
        #         bv  <- nzidx
        #         bs  <- (x[i] + x[min(i+1,ndSize)]) / 2
        #     }
        # }
    }
    return(list(MaxdeltaI = maxdI, BestVar = bv, BestSplit = bs))
}

split.mse <- function(Y) {
    split.mean <- mean(Y)
    return(sum((split.mean - Y)^2))
}

