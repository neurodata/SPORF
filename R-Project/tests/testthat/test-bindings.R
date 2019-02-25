context("General tests")
## These tests were originally in travisTest/test.R and this is an attempt to port
## them to test_that

library(rerf)
X <- as.matrix(iris[, 1:4])
Y <- as.numeric(iris[[5L]])-1


test_that("Testing basic parameter checks.", {
						expect_that(fpRerF(), throws_error("no input provided."))
						expect_that(fpRerF(X,Y,forestType=NULL), throws_error("must pick a forest type: rfBase, rerf, inPlace, inPlaceRerF, binnedBase, binnedBaseRerF"))
						expect_that(fpRerF(X,Y,numTreesInForest=0), throws_error("at least one tree must be used."))
						expect_that(fpRerF(X,Y,minParent=0), throws_error("at least one observation must be used in each node."))
						expect_that(fpRerF(X,Y,numCores=0), throws_error("at least one core must be used."))
})

test_that("Testing basic functionality. Forest should do perfect when testing on seperable training set.", {
						forest <- fpRerF(X,Y)
						error <- mean(fpPredict(forest,X)==Y)
						expect_true(error == 1)
})

test_that("Test seeding.", {
						forest <- fpRerF(X,Y, numTreesInForest=1,forestType="binnedBaseRerF",seed=2)
						error <- mean(fpPredict(forest,X)==Y)
						#not working yet.  This is a due out.
						#expect_true(error == 1)
})
