context("RandMat* tests")
library(rerf)


test_that("Testing RandMat", {

})



test_that("Testing RandMatBinary", {
  paramList <- list(p = 100, d = 25, sparsity = 2/10, prob = 0.5)
  a <- do.call(RandMatBinary, paramList)

  ## Check that columns are in the correct space
  expect_true(all(a[, 1] %in% 1:paramList$p))
  expect_true(all(a[, 2] %in% 1:paramList$d))

  ## Check that weights are the correct proportions
  bi <- binom.test(sum(a[, 3] == 1), nrow(a), paramList$prob, "two")

  expect_true(bi$p.value >= 0.05)
})


test_that("Testing RandMatContinuous", {
  paramList <- list(p = 100, d = 25, sparsity = 2/10)
  a <- do.call(RandMatContinuous, paramList)

  ## Check that columns are in the correct space
  expect_true(all(a[, 1] %in% 1:paramList$p))
  expect_true(all(a[, 2] %in% 1:paramList$d))

  ## Check that weights are the correct proportions
  ks <- ks.test(a[, 3], "pnorm", 0,1)
  expect_true(ks$p.value >= 0.05)
})



test_that("Testing RandMatPoisson", {
  paramList <- list(p = 100, d = 25, lambda = 5)
  a <- do.call(RandMatPoisson, paramList)

  ## Check that columns are in the correct space
  expect_true(all(a[, 1] %in% 1:paramList$p))
  expect_true(all(a[, 2] %in% 1:paramList$d))

  ## Check that weights are the correct proportions
  ##TODO: Not sure what to do here yet. to check for ~ Poisson
  ## See https://github.com/neurodata/RerF/issues/208

  ## Check that the weights are ~ Binomial(0.5)
  bi <- binom.test(sum(a[, 3] == 1), nrow(a), 0.5, "two")
  expect_true(bi$p.value >= 0.05)
})


test_that("Testing RandMatFRC", {
  paramList <- list(p = 100, d = 25, nmix = 5)
  a <- do.call(RandMatFRC, paramList)

  ## Check that columns are in the correct space
  expect_true(all(a[, 1] %in% 1:paramList$p))
  expect_true(all(a[, 2] %in% 1:paramList$d))

  ## Check that the weights are ~ U[-1,1]
  ks <- ks.test(x = a[, 3], y = "punif", min = -1, max = 1)
  expect_true(ks$p.value >= 0.05)
})


test_that("Testing RandMatFRCN", {
  paramList <- list(p = 100, d = 25, nmix = 5)
  a <- do.call(RandMatFRCN, paramList)

  ## Check that columns are in the correct space
  expect_true(all(a[, 1] %in% 1:paramList$p))
  expect_true(all(a[, 2] %in% 1:paramList$d))

  ## Check that the weights are ~ N(0,1)
  ks <- ks.test(x = a[, 3], y = "pnorm", mean = 0, sd = 1)
  expect_true(ks$p.value >= 0.05)
})


test_that("Testing RandMatImagePatch", {
  paramList <- list(p = 32^2, d = 32, ih = 32, iw = 32, pwMin = 2, pwMax = 16)
  a <- do.call(RandMatImagePatch, paramList)

  ## Check that columns are in the correct space
  expect_true(all(a[, 1] %in% 1:paramList$p))
  expect_true(all(a[, 2] %in% 1:paramList$d))

  ## Check that the weights are all 1
  expect_true(all(a[, 3] == 1))

  ##TODO: could also check for height and width here
})


test_that("Testing RandMatImageControl", {
  paramList <- list(p = 32^2, d = 32, ih = 32, iw = 32, pwMin = 2, pwMax = 16)
  a <- do.call(RandMatImageControl, paramList)

  ## Check that columns are in the correct space
  expect_true(all(a[, 1] %in% 1:paramList$p))
  expect_true(all(a[, 2] %in% 1:paramList$d))

  ## Check that the weights are all 1
  expect_true(all(a[, 3] == 1))

  ##TODO: could also check for height and width here
})



## temporary fix for R-dev
suppressWarnings(RNGversion("3.5.0"))

test_that("Testing RandMat binary option.", {
  ## below is the output of RandMat with mat.options
  ## from commit 73b896ff053537ee23d82b9debee054171b1c41b
  ## with set.seed(317) and RcppZiggurat::zsetseed(14)
  ## for comparison to the new version of RandMat*
  # mat.options <- list(p = 5, d = 3, "binary", rho = 0.25, prob = 0.5)
  rBinary <- structure(c(3, 2, 3, 2, 1, 2, 2, 3, 1, -1, -1, 1), .Dim = 4:3)

  set.seed(317)
  RcppZiggurat::zsetseed(14)
  # optBinary <- list(p = 5, d = 3, "binary", rho = 0.25, prob = 0.5)
  # a01 <- RandMat(optBinary)
  optBinary <- list(p = 5, d = 3, sparsity = 0.25, prob = 0.5)
  a01 <- do.call(RandMatBinary, optBinary)
  expect_equal(a01, rBinary)
})


test_that("Testing RandMat continuous option.", {
  ## below is the output of RandMat with mat.options
  ## from commit 73b896ff053537ee23d82b9debee054171b1c41b
  ## with set.seed(317) and RcppZiggurat::zsetseed(14)
  ## for comparison to the new version of RandMat*
  # mat.options <- list(p = 5, d = 3, "continuous", rho = 0.25, prob = 0.5)
  rContinuous <- structure(c(
    3, 2, 3, 2, 1, 2, 2, 3, -0.42934944005662,
    -1.00656832180404, -0.346220301300449,
    -2.88694544455467
  ), .Dim = 4:3)
  set.seed(317)
  RcppZiggurat::zsetseed(14)
  # optContinuous <- list(p = 5, d = 3, "continuous", rho = 0.25, prob = 0.5)
  # a02 <- RandMat(optContinuous)
  optContinuous <- list(p = 5, d = 3, sparsity = 0.25)
  a02 <- do.call(RandMatContinuous, optContinuous)
  expect_equal(a02, rContinuous)
})


test_that("Testing RandMat rf option.", {
  ## below is the output of RandMat with mat.options
  ## from commit 73b896ff053537ee23d82b9debee054171b1c41b
  ## with set.seed(317) and RcppZiggurat::zsetseed(14)
  ## for comparison to the new version of RandMat*
  # mat.options <- list(p = 5, d = 3, "rf")
  rRF <- structure(c(3L, 1L, 2L, 1L, 2L, 3L, 1L, 1L, 1L), .Dim = c(3L, 3L))

  set.seed(317)
  RcppZiggurat::zsetseed(14)
  # optRF <- list(p = 5, d = 3, "rf")
  # a03 <- RandMat(optRF)
  optRF <- list(p = 5, d = 3)
  a03 <- do.call(RandMatRF, optRF)
  expect_equal(a03, rRF)
})


test_that("Testing RandMat poisson option.", {
  ## below is the output of RandMat with mat.options
  ## from commit 73b896ff053537ee23d82b9debee054171b1c41b
  ## with set.seed(317) and RcppZiggurat::zsetseed(14)
  ## for comparison to the new version of RandMat*
  # mat.options <- list(p = 5, d = 3, "poisson", lambda = 0.25)
  rPoisson <- structure(c(1L, 3L, 1L, 1L, -1L, -1L), .Dim = 2:3, .Dimnames = list(NULL, c("nz.rows", "nz.cols", "")))

  set.seed(317)
  RcppZiggurat::zsetseed(14)
  # optPoisson <- list(p = 5, d = 3, "poisson", lambda = 0.25)
  # a04 <- RandMat(optPoisson)
  optPoisson <- list(p = 5, d = 3, lambda = 0.25)
  a04 <- do.call(RandMatPoisson, optPoisson)
  expect_equal(a04, rPoisson)
})


test_that("Testing RandMat frc option.", {
  ## below is the output of RandMat with mat.options
  ## from commit 73b896ff053537ee23d82b9debee054171b1c41b
  ## with set.seed(317) and RcppZiggurat::zsetseed(14)
  ## for comparison to the new version of RandMat*
  # mat.options <- list(p = 5, d = 3, "frc", nmix = 3)
  rFRC <- structure(c(
    3, 1, 2, 5, 3, 1, 1, 3, 5, 1, 1, 1, 2, 2, 2, 3, 3,
    3, -0.627921188715845, 0.229841713327914,
    0.84218871453777, 0.40449609188363,
    0.116797041147947, 0.42892448650673,
    0.101170502603054, 0.918304143939167,
    -0.278529048897326
  ),
  .Dim = c(9L, 3L),
  .Dimnames = list(NULL, c("nz.rows", "nz.cols", ""))
  )

  set.seed(317)
  RcppZiggurat::zsetseed(14)
  # optFRC <- list(p = 5, d = 3, "frc", nmix = 3)
  # a05 <- RandMat(optFRC)
  optFRC <- list(p = 5, d = 3, nmix = 3)
  a05 <- do.call(RandMatFRC, optFRC)
  expect_equal(a05, rFRC)
})


test_that("Testing RandMat frcn option.", {
  ## below is the output of RandMat with mat.options
  ## from commit 73b896ff053537ee23d82b9debee054171b1c41b
  ## with set.seed(317) and RcppZiggurat::zsetseed(14)
  ## for comparison to the new version of RandMat*
  # mat.options <- list(p = 5, d = 3, "frcn", nmix = 3)
  rFRCN <- structure(c(
    3, 1, 2, 5, 3, 1, 1, 3, 5, 1, 1, 1, 2, 2, 2, 3,
    3, 3, -0.42934944005662, -1.00656832180404,
    -0.346220301300449, -2.88694544455467,
    -0.460375256980165, -0.410029765762874,
    -1.98434996875784, 2.33299111391622,
    1.17541739193505
  ),
  .Dim = c(9L, 3L),
  .Dimnames = list(NULL, c("nz.rows", "nz.cols", ""))
  )

  set.seed(317)
  RcppZiggurat::zsetseed(14)
  # optFRCN <- list(p = 5, d = 3, "frcn", nmix = 3)
  # a07 <- RandMat(optFRCN)
  optFRCN <- list(p = 5, d = 3, nmix = 3)
  a07 <- do.call(RandMatFRCN, optFRCN)
  expect_equal(a07, rFRCN)
})


test_that("Testing RandMat ts-patch option.", {
  ## below is the output of RandMat with mat.options
  ## from commit 73b896ff053537ee23d82b9debee054171b1c41b
  ## with set.seed(317) and RcppZiggurat::zsetseed(14)
  ## for comparison to the new version of RandMat*
  # mat.options <- list(p = 5, d = 3, "ts-patch", pw.min = 3, pw.max = 8)
  rTS <- structure(c(
    10L, 6L, 7L, 8L, 9L, 4L, 5L, 6L, 7L, 8L, 9L, 1L,
    2L, 2L, 2L, 2L, 3L, 3L, 3L, 3L, 3L, 3L, 1L, 1L, 1L,
    1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L
  ),
  .Dim = c(11L, 3L),
  .Dimnames = list(NULL, c("nz.rows", "nz.cols", ""))
  )
  set.seed(317)
  RcppZiggurat::zsetseed(14)
  # optTS <- list(p = 10, d = 3, "ts-patch", pw.min = 3, pw.max = 8)
  # a08 <- RandMat(optTS)
  optTS <- list(p = 10, d = 3, pwMin = 3, pwMax = 8)
  a08 <- do.call(RandMatTSpatch, optTS)
  expect_equal(a08, rTS)
})


test_that("Testing RandMat image-patch option.", {
  ## below is the output of RandMat with mat.options
  ## from commit 73b896ff053537ee23d82b9debee054171b1c41b
  ## with set.seed(317) and RcppZiggurat::zsetseed(14)
  ## for comparison to the new version of RandMat*
  # mat.options <- list(p = 100, d = 3, "image-patch", iw = 10, ih = 10, pw.min = 3, pw.max = 8)
  rIP <- structure(c(
    3, 4, 5, 6, 7, 13, 14, 15, 16, 17, 23, 24, 25, 26,
    27, 33, 34, 35, 36, 37, 43, 44, 45, 46, 47, 53, 54,
    55, 56, 57, 63, 64, 65, 66, 67, 73, 74, 75, 76, 77,
    36, 37, 38, 46, 47, 48, 56, 57, 58, 66, 67, 68, 76,
    77, 78, 86, 87, 88, 22, 23, 24, 25, 26, 27, 32, 33,
    34, 35, 36, 37, 42, 43, 44, 45, 46, 47, 52, 53, 54,
    55, 56, 57, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
  ),
  .Dim = c(82L, 3L),
  .Dimnames = list(NULL, c("nz.rows", "nz.cols", ""))
  )

  set.seed(317)
  RcppZiggurat::zsetseed(14)
  # optIP <- list(p = 100, d = 3, "image-patch", iw = 10, ih = 10, pw.min = 3, pw.max = 8)
  # a09 <- RandMat(optIP)
  optIP <- list(p = 100, d = 3, iw = 10, ih = 10, pwMin = 3, pwMax = 8)
  a09 <- do.call(RandMatImagePatch, optIP)
  expect_equal(a09, rIP)
})


test_that("Testing RandMat image-control option.", {
  ## below is the output of RandMat with mat.options
  ## from commit 73b896ff053537ee23d82b9debee054171b1c41b
  ## with set.seed(317) and RcppZiggurat::zsetseed(14)
  ## for comparison to the new version of RandMat*
  # mat.options <- list(p = 100, d = 3, "image-control", iw = 10, ih = 10, pw.min = 3, pw.max = 8)
  rIC <- structure(c(
    14L, 73L, 33L, 19L, 60L, 88L, 67L, 52L, 66L, 51L,
    87L, 98L, 43L, 70L, 63L, 46L, 45L, 2L, 18L, 41L,
    93L, 12L, 40L, 80L, 75L, 16L, 65L, 92L, 95L, 79L,
    38L, 22L, 94L, 24L, 37L, 85L, 15L, 42L, 28L, 100L,
    22L, 73L, 82L, 90L, 68L, 17L, 99L, 89L, 77L, 45L,
    25L, 51L, 98L, 37L, 65L, 85L, 74L, 42L, 35L, 39L,
    87L, 85L, 24L, 68L, 20L, 19L, 82L, 49L, 77L, 73L,
    86L, 71L, 4L, 50L, 13L, 92L, 28L, 11L, 54L, 38L,
    16L, 90L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L,
    1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L,
    1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L,
    1L, 1L, 1L, 1L, 2L, 2L, 2L, 2L, 2L, 2L, 2L, 2L, 2L,
    2L, 2L, 2L, 2L, 2L, 2L, 2L, 2L, 2L, 3L, 3L, 3L, 3L,
    3L, 3L, 3L, 3L, 3L, 3L, 3L, 3L, 3L, 3L, 3L, 3L, 3L,
    3L, 3L, 3L, 3L, 3L, 3L, 3L, 1L, 1L, 1L, 1L, 1L, 1L,
    1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L,
    1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L,
    1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L,
    1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L,
    1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L,
    1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L
  ),
  .Dim = c(82L, 3L),
  .Dimnames = list(NULL, c("nz.rows", "nz.cols", ""))
  )

  set.seed(317)
  RcppZiggurat::zsetseed(14)
  # optIC <- list(p = 100, d = 3, "image-control", iw = 10, ih = 10, pw.min = 3, pw.max = 8)
  # a10 <- RandMat(optIC)
  optIC <- list(p = 100, d = 3, iw = 10, ih = 10, pwMin = 3, pwMax = 8)
  a10 <- do.call(RandMatImageControl, optIC)
  expect_equal(a10, rIC)
})


test_that("Testing RandMat custom option.", {
  ## below is the output of RandMat with mat.options
  ## from commit 73b896ff053537ee23d82b9debee054171b1c41b
  ## with set.seed(317) and RcppZiggurat::zsetseed(14)
  ## for comparison to the new version of RandMat*
  # mat.options <- list(p = 10, d = 3, "custom", nnz.sample = 5, nnz.prob = rep(1, 5))
  rC <- structure(c(
    10, 6, 3, 1, 8, 3, 2, 6, 8, 5, 1, 1, 1, 1, 2, 2, 3,
    3, 3, 3, -0.42934944005662, -1.00656832180404,
    -0.346220301300449, -2.88694544455467,
    -0.460375256980165, -0.410029765762874,
    -1.98434996875784, 2.33299111391622,
    1.17541739193505, 0.241768406815464
  ),
  .Dim = c(10L, 3L),
  .Dimnames = list(NULL, c("nz.rows", "nz.cols", ""))
  )

  set.seed(317)
  RcppZiggurat::zsetseed(14)
  # optC <- list(p = 10, d = 3, "custom", nnz.sample = 5, nnz.prob = rep(1, 5))
  # a11 <- RandMat(optC)
  optC <- list(p = 10, d = 3, nnzSample = 5, nnzProb = rep(1, 5))
  a11 <- do.call(RandMatCustom, optC)
  expect_equal(a11, rC)
})


test_that("Testing RandMat makeAB for Urerf option.", {
  numFeatures <- 10
  numToTry <- 5
  mAB <- rerf:::makeAB(numFeatures, numToTry, 1 / numFeatures)


  expect_true(min(mAB[, 1]) > 0)
  expect_true(max(mAB[, 1]) <= numFeatures)
  expect_true(min(mAB[, 2]) > 0)
  expect_true(max(mAB[, 2]) <= numToTry)
  expect_true(ncol(mAB) == 3)
  expect_true(nrow(mAB) == numToTry)
  expect_true(all(unique(mAB[, 3]) == -1 | unique(mAB[, 3]) == 1))
})


test_that("Testing RandMat makeA for Urerf option.", {
  numFeatures <- 10
  numToTry <- 5
  mAB <- rerf:::makeA(numFeatures, numToTry, 1 / numFeatures)


  expect_true(min(mAB[, 1]) > 0)
  expect_true(max(mAB[, 1]) <= numFeatures)
  expect_true(min(mAB[, 2]) == 1)
  expect_true(max(mAB[, 2]) == numToTry)
  expect_true(sum(mAB[, 2]) == numToTry * (numToTry + 1) / 2)
  expect_true(ncol(mAB) == 3)
  expect_true(nrow(mAB) == numToTry)
  expect_true(unique(mAB[, 3]) == 1)
})

## reset RNG to current version
si <- sessionInfo()
vstr <- paste0(si$R.version$major,".", si$R.version$minor)
RNGversion(vstr)
