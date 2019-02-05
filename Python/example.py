from RerF import fastRerF, fastPredict
import numpy as np


datafile = "../src/packedForest/res/iris.csv"
# datafile = "../src/packedForest/res/mnist.csv"

label_col = 4  # iris
# label_col = 0  # mnist

print("loading data...")
X = np.genfromtxt(datafile, delimiter=",")
print("data loaded")

feat_data = X[:, 0:4]  # iris
# feat_data = X[:, 1:]  # mnist

forest = fastRerF(
    CSVFile=datafile, Ycolumn=label_col, forestType="rerf", trees=1, seed=1, numCores=4
)

forest.printParameters()

predictions = fastPredict(feat_data, forest)
print(predictions)

print(
    "Error rate",
    sum([int(p) != int(i) for p, i in zip(predictions, X[:, label_col])]) / X.shape[0],
)

print("loading test data...")
test_X = np.genfromtxt("../src/packedForest/res/iris.csv", delimiter=",")  # iris
# test_X = np.genfromtxt("../src/packedForest/res/mnist_test.csv", delimiter=",") #mnist
print("data loaded")

test_data = test_X[:, 0:4]  # iris
# test_data = test_X[:, 1:]  # mnist
test_pred = fastPredict(test_data, forest)

print(
    "Error rate (test data)",
    sum([int(p) != int(i) for p, i in zip(test_pred, test_X[:, label_col])])
    / X.shape[0],
)
