if(file.exists("src/RcppExports.o"))file.remove("src/RcppExports.o")
if(file.exists("src/split.o"))file.remove("src/split.o")
if(file.exists("src/forestPacking.o"))file.remove("src/forestPacking.o")
if(file.exists("src/forestPacking/treeStruct/treeBin2.o"))file.remove("src/forestPacking/treeStruct/treeBin2.o")

if(file.exists("src/rerf.so"))file.remove("src/rerf.so")
if(file.exists("src/forestPacking/improv8.o"))file.remove("src/forestPacking/improv8.o")
if(file.exists("src/forestPacking/treeStruct/inferenceSamples.o"))file.remove("src/forestPacking/treeStruct/inferenceSamples.o")
if(file.exists("src/forestPacking/treeStruct/padForest.o"))file.remove("src/forestPacking/treeStruct/padForest.o")
if(file.exists("src/forestPacking/treeStruct/padNode.o"))file.remove("src/forestPacking/treeStruct/padNode.o")
if(file.exists("src/forestPacking/treeStruct/padNodeStat.o"))file.remove("src/forestPacking/treeStruct/padNodeStat.o")

library(Rcpp)
compileAttributes()

library(devtools)
install.packages(".", repos=NULL)
