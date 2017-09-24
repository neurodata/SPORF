RankMatrix <-
    function(X, na.last = T, ties.method = "average") {
        if (is.matrix(X)) {
            X.rank <- apply(X, 2, FUN = function(x) rank(x, na.last = na.last, ties.method = ties.method))
        } else {
            X.rank <- rank(X, na.last = na.last, ties.method = ties.method)
        }
        return(X.rank)
    }
