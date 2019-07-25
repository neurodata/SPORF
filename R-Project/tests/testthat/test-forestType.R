context("Forest Types")
library(rerf)

# Get iris
X <- as.matrix(iris[, 1:4])
Y <- as.numeric(iris[[5L]]) - 1

test_that("Test setting forestType", {
	expect_warning(
		expect_error(
			fpRerF(X, Y, forestType = "inPlace")
		),
		"Using non-standard forestType inPlace."
	)
})
