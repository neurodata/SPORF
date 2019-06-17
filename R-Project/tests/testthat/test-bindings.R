context("General tests")
## These tests were originally in travisTest/test.R and this is an attempt to port
## them to test_that

library(rerf)
X <- as.matrix(iris[, 1:4])
Y <- as.numeric(iris[[5L]])-1


test_that("Testing basic parameter checks in bindings.", {
						expect_that(fpRerF(), throws_error("no input provided."))

						expect_warning(
							expect_that(fpRerF(X,Y,forestType="blah"), throws_error("Unimplemented forest type chosen.")),
							"Using non-standard forestType blah."
						)

						expect_that(fpRerF(X,Y,numTreesInForest=0), throws_error("at least one tree must be used."))
						expect_that(fpRerF(X,Y,minParent=0), throws_error("at least one observation must be used in each node."))
						expect_that(fpRerF(X,Y,numCores=0), throws_error("at least one core must be used."))
})

test_that("Testing basic functionality. Forest should do almost perfect on iris.", {
						invisible(forest <- fpRerF(X,Y, seed = 1))
						error <- mean(fpPredict(forest,X) != Y)
						expect_true(error <= 2/150)
})
