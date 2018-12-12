library(rerf)
zs <- fastRerF("data/iris.csv", 4, forestType="rfBase")
#zs <- fastRerF("data/iris.csv", 4, forestType="rerf")
zs$printParameters();
zs$printForestType();


predictions <- fastPredict(as.matrix(iris[,1:4]))
print(predictions)
