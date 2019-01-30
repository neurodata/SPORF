import pyfp

forest = pyfp.fpForest()
forest.setParameter("forestType", "rerf")

# forest.setParameter("CSVFileName", "res/iris.csv")
# forest.setParameter("columnWithY", 4)

# forest.setParameter("CSVFileName", "res/higgs2.csv")
# forest.setParameter("columnWithY", 0)

forest.setParameter("CSVFileName", "src/packedForest/res/mnist.csv")
forest.setParameter("columnWithY", 0)

forest.setParameter("numTreesInForest", 100)

forest.setParameter("minParent", 1)
forest.setParameter("numCores", 1)

print("Parameters:\n")
forest.printParameters()
forest.printForestType()

forest.growForest()

print("\ntesting:")
print(forest.testAccuracy())
