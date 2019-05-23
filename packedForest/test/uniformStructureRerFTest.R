

h <- read.table('heights.txt')[[1]]
w <- read.table('widths.txt')[[1]]
tl <- read.table('topLeftVec.txt')[[1]]

X <- data.frame(h,w,tl)


if ( wilcox.test(h, length(h) / max(h))$p.value >= 0.1){
	warning("p-value >= 0.1")
}


if ( wilcox.test(w, length(w) / max(w))$p.value >= 0.1){
	warning("p-value >= 0.1")
}



