



findSplitRF <- function(x,
                        y,
                        ndSize,
                        I,
                        maxdI,
                        bv,
                        bs,
                        nzidx) {
    for (i in 1:ndSize) {
        dI <- I - sum(c(split.mse(y[1:i]), split.mse(y[-(1:i)])))
        if (dI > maxdI) {
            maxdI <- dI
            bv  <- nzidx
            bs  <- (x[i] + x[min(i+1,ndSize)]) / 2
        }
    }
    return(list(MaxdeltaI = maxdI, BestVar = bv, BestSplit = bs))
}

split.mse <- function(Y) {
    split.mean <- mean(Y)
    return(sum((split.mean - Y)^2))
}

