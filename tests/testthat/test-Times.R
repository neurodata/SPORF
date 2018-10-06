context("Timing tests of multiple versions of lumbeRjack.")

times <- list()

test_that("Timed v1.1.3", {
  skip("Timed tests are not run automatically.")

  require(devtools)

  ## sandbox the install location
  dev_mode(on = TRUE) 

  ## install from version 1.1.3 which is the CRAN version as of 20181005.
  install_github('neurodata/lumberjack', ref = 'v1.1.3', local = FALSE)
  require('rerf')
 
  data(mnist)

  ## Get a random subsample, 100 each of 3's and 5's 
  set.seed(317)
  threes <- sample(which(mnist$Ytrain %in% 3), 100)
  fives  <- sample(which(mnist$Ytrain %in% 5), 100)
  numsub <- c(threes, fives)
  
  Ytrain <- mnist$Ytrain[numsub]
  Xtrain <- mnist$Xtrain[numsub,]
  Ytest <- mnist$Ytest[mnist$Ytest %in% c(3,5)]
  Xtest <- mnist$Xtest[mnist$Ytest %in% c(3,5),]
  
  # p is number of dimensions, d is the number of random features to evaluate, iw is image width, ih is image height, patch.min is min width of square patch to sample pixels from, and patch.max is the max width of square patch
  p <- ncol(Xtrain)
  d <- ceiling(sqrt(p))
  iw <- sqrt(p)
  ih <- iw
  patch.min <- 1L
  patch.max <- 5L

  startTime <- Sys.time()

  forest <- RerF(Xtrain, Ytrain, num.cores = 1L, 
                 mat.options = list(p = p, d = d, random.matrix = "image-patch", iw = iw, ih = ih, 
                                    patch.min = patch.min, patch.max = patch.max), 
                 seed = 1L, rfPack = FALSE)
  stopTime <- Sys.time()
  
  times$cran <- stopTime - startTime

  #################################################################
  #####                install from staging.                  #####
  #################################################################

  detach('package:rerf', unload = TRUE)
  install_github('neurodata/lumberjack', ref = 'staging', local = FALSE)
  require('rerf')
 
  startTime <- Sys.time()
  forest <- RerF(Xtrain, Ytrain, num.cores = 1L, 
                 mat.options = list(p = p, d = d, random.matrix = "image-patch", iw = iw, ih = ih, 
                                    patch.min = patch.min, patch.max = patch.max), 
                 seed = 1L)
  stopTime <- Sys.time()
  times$staging <- stopTime - startTime
  
  
  
  #################################################################
  #####                install from RandMat-split             #####
  #################################################################
  
  
  ## install from branch RandMat-split.
  detach('package:rerf', unload = TRUE)
  install_github('neurodata/lumberjack', ref = 'RandMat-split', local = FALSE)
  require('rerf')
 
  startTime <- Sys.time()
  forest <- RerF(Xtrain, Ytrain, num.cores = 1L, FUN = RandMatImagePatch,
               paramList = list(p = p, d = d, iw = iw, ih = ih, 
                                pwMin = patch.min, pwMax = patch.max), 
               seed = 1L)
  stopTime <- Sys.time()
  times$randMatSplit <- stopTime - startTime

  print(times)
  dev_mode(on = FALSE)
})



test_that("Timing RandMat binary option.", {
  skip("Timed tests are not run automatically.")

  runs <- list()
  require(microbenchmark)
  ## below is the output of RandMat with mat.options
  ## from commit 73b896ff053537ee23d82b9debee054171b1c41b
  ## with set.seed(317) and RcppZiggurat::zsetseed(14)
  ## for comparison to the new version of RandMat*
  #mat.options <- list(p = 5, d = 3, "binary", rho = 0.25, prob = 0.5)

  ## sandbox the install location
  dev_mode(on = TRUE) 

  ## install from version 1.1.3 which is the CRAN version as of 20181005.
  install_github('neurodata/lumberjack', ref = 'v1.1.3', local = FALSE, force = TRUE)
  require('rerf')

  rBinary <- structure(c(3, 2, 3, 2, 1, 2, 2, 3, 1, -1, -1, 1), .Dim = 4:3)

  opt1 <- list(p = 5, d = 3, random.matrix = "binary", rho = 0.25, prob = 0.5)
  opt2 <- list(p = 5, d = 3, rho = 0.25, prob = 0.5)
 
  runs$cran <- microbenchmark(run1 = RandMat(opt1))

  ## install from branch RandMat-split
  detach('package:rerf', unload = TRUE)
  install_github('neurodata/lumberjack', ref = 'RandMat-split', local = FALSE, force = TRUE)
  require('rerf')

  runs$randmat <- microbenchmark(run2 = RandMatBinary(p = 5, d = 3, rho = 0.25, prob = 0.5))

  dev_mode(on = FALSE)
})


