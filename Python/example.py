from RerF import fastRerF, fastPredict
import numpy as np
from multiprocessing import cpu_count

# datatype = "iris"
datatype = "mnist"

if datatype == "iris":
    datafile = "../packedForest/res/iris.csv"
    label_col = 4
elif datatype == "mnist":
    datafile = "../packedForest/res/mnist.csv"
    label_col = 0

print("loading data...")
X = np.genfromtxt(datafile, delimiter=",")
print("data loaded")

if datatype == "iris":
    feat_data = X[:, 0:4]  # iris
    labels = X[:, 4]
elif datatype == "mnist":
    feat_data = X[:, 1:]  # mnist
    labels = X[:, 0]

# forest = fastRerF(
#     CSVFile=datafile, Ycolumn=label_col, forestType="rerf", trees=1, seed=1, numCores=cpu_count()-1
# )
forest = fastRerF(
    X=feat_data,
    Y=labels,
    forestType="rerf",
    trees=500,
    seed=1,
    numCores=cpu_count() - 1,
)

forest.printParameters()

predictions = fastPredict(feat_data, forest)
print(predictions)

print(
    "Error rate",
    sum([int(p) != int(i) for p, i in zip(predictions, X[:, label_col])]) / X.shape[0],
)

print("loading test data...")

if datatype == "iris":
    test_X = np.genfromtxt("../packedForest/res/iris.csv", delimiter=",")  # iris
elif datatype == "mnist":
    test_X = np.genfromtxt("../packedForest/res/mnist_test.csv", delimiter=",")  # mnist
print("data loaded")

if datatype == "iris":
    test_data = test_X[:, 0:4]  # iris
elif datatype == "mnist":
    test_data = test_X[:, 1:]  # mnist

test_pred = fastPredict(test_data, forest)

print(
    "Error rate (test data)",
    sum([int(p) != int(i) for p, i in zip(test_pred, test_X[:, label_col])])
    / X.shape[0],
)
