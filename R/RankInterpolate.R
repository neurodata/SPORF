RankInterpolate <-
    function(Xtrain, Xtest) {
        Xrank <- sapply(seq.int(ncol(Xtest)), FUN = function(cl) RankInterpolateVector(Xtrain[, cl], Xtest[, cl]))
        return(Xrank)
    }
