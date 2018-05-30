library(randomForest)
library(slb)

get_cv_results <- function(algorithm, dataset, folds_i, d = 0, rho=0) {
    folds_k <- config::get("k_folds")
    cv_results <- numeric(folds_k)
    for (k in 1:folds_k) {
        p <- dim(dataset)[2]
        best_result <- NULL
        train_fold <- get_fold(which(folds_i != k), dataset)
        test_fold <- get_fold(which(folds_i == k), dataset)
        algorithm_predictions <- run_algorithm(algorithm,
                                               train_fold,
                                               test_fold,
                                               d = d,
                                               rho = rho
                                               
        )
        scaling <- var(dataset[,dim(dataset)[2]])
        cv_results[k] <-
            compute.nmse(algorithm_predictions, test_fold$Y,scaling) * 100
        if (verbose == TRUE)
        {
            if (algorithm == "RerF") {
                progress_string <- sprintf(
                    "(Dataset) %s \t (Algorithm) %s \t d=%f, rho=%f \t on fold %i: %s",
                    dataset_name,
                    algorithm,
                    d,
                    rho,
                    k,
                    numeric2string(cv_results[k])
                )
            } else {
                progress_string <- sprintf(
                    "(Dataset) %s \t (Algorithm) %s \t on fold %i: %s",
                    dataset_name,
                    algorithm,
                    k,
                    numeric2string(cv_results[k])
                )
            }
            
            cat(progress_string,'\n')
        }
    }
    return(cv_results)
}


run_signed_rank_test <-
    function(results_matrix, algorithm1, algorithm2) {
        # TODO: Undo as.character if it causes a bug
        results1 <-
            sapply(strsplit(as.character(results_matrix[, algorithm1]), "+/-", fixed = TRUE), function(x)
                as.numeric(x[1]))
        results2 <-
            sapply(strsplit(as.character(results_matrix[, algorithm2]), "+/-", fixed = TRUE), function(x)
                as.numeric(x[1]))
        test_result <-
            wilcox.test(results1,
                        results2,
                        alternative = "greater",
                        paired = TRUE)
        if (config::get("verbose")) {
            if (test_result$p.value < 0.05) {
                pass <- "SIGNIFICANT"
            } else {
                pass <- "NOT SIGNIFICANT"
            }
            cat(
                sprintf(
                    "Sign test between %s and %s is %s with pval %.4f\n",
                    algorithm1,
                    algorithm2,
                    pass,
                    test_result$p.value
                )
            )
        }
        return(test_result)
    }

final_result_string <-
    function(mu, sd)
        paste0(numeric2string(mu), ' +/- ', numeric2string(sd))

save_checkpoint <- function(results_matrix) {
    base_dir = config::get("base_dir")
    rda_checkpoint_file <-
        paste0(base_dir, "results/weka_benchmarks.rda")
    csv_checkpoint_file <-
        paste0(base_dir, "results/weka_benchmarks.csv")
    save(results_matrix, file = rda_checkpoint_file)
    write.csv(results_matrix, file = csv_checkpoint_file)
}

preprocess <- function(dataset) {
    p <- dim(dataset)[2]
    # Normalize data
    numeric_columns <- sapply(dataset[,-p],
                              function(x) {
                                  if (is.numeric(x)) {
                                      TRUE
                                  } else {
                                      FALSE
                                  }
                              })
    numeric_columns <- unname(which(numeric_columns == TRUE))
    dataset[, numeric_columns] <-
        data.frame(sapply(dataset[, numeric_columns],
                          function(x) {
                              if (is.numeric(x) == TRUE) {
                                  scaled_x <- scale(x)
                                  attr(scaled_x, "scaled:scale") <-
                                      NULL
                                  attr(scaled_x, "scaled:center") <-
                                      NULL
                                  return(scaled_x)
                              } else {
                                  return(x)
                              }
                          }))
    
    dataset_mode <- config::get("dataset_mode")
    if (dataset_mode == 'ccfs') {
        p <- dim(dataset)[2]
        X <- as.matrix(dataset[,-p])
        Y <- as.matrix(dataset[, p])
    } else if (dataset_mode == 'weka') {
        
    } else {
        stop()
    }
    
    # Deal with missing values
    na_mode <- config::get("na_mode")
    if (na_mode == "remove") {
        complete_samples <- complete.cases(dataset)
        dataset <- dataset[complete_samples,]
    } else if (na_mode == "sample") {
        missing_X <- which(is.na(X), arr.ind = TRUE)
        dataset[missing_X] <-
            sapply(1:ncol(missing_X), function(x)
                rnorm(1))
    } else {
        stop()
    }
    
    return(dataset)
}

get_fold <- function(indices, dataset) {
    p <- dim(dataset)[2]
    dataset_mode = config::get("dataset_mode")
    if (dataset_mode == 'ccfs') {
        p <- dim(dataset)[2]
        X <- dataset[indices, -p]
        Y <- dataset[indices, p]
        list(X = X, Y = Y)
    } else if (dataset_mode == 'weka') {
        
    } else {
        stop()
    }
}

encode_factors <- function(features) {
    p <- dim(features)[2]
    cat.map <- list()
    features_type <- sapply(features[,],
                            function(x) {
                                if (is.numeric(x)) {
                                    FALSE
                                } else {
                                    TRUE
                                }
                            })
    categorical_features <- unname(which(features_type == TRUE))
    numerical_features <- unname(which(features_type == FALSE))
    if (length(categorical_features) == 0) {
        return(list(X = as.matrix(features), cat.map = NULL))
    }
    
    encoded_features <- list()
    cat_start <- 1L
    sapply(categorical_features, function(v) {
        cat_names <- levels(features[, v])
        unique_cat_names <- c()
        encoded_features <- list()
        for (cat_val in cat_names) {
            unique_cat_val <- paste0(v, '.', cat_val)
            encoded_features[[unique_cat_val]] <<-
                as.integer(features[, v] == cat_val)
        }
        cat_end <- cat_start + length(cat_names) - 1
        cat.map <<- append(cat.map, list(cat_start:cat_end))
        cat_start <<- cat_end + 1
    })
    X <-
        cbind(features[, numerical_features], data.frame(encoded_features))
    return(list(X = as.matrix(X),
                cat.map = cat.map))
}

run_algorithm <-
    function(algorithm, train_fold, test_fold, d=0, rho=0) {
        p <- dim(train_fold$X)[2]
        if (algorithm == "RF") {
            rf_model <- randomForest(
                x = train_fold$X,
                y = train_fold$Y,
                xtest = test_fold$X,
                ytest = test_fold$Y,
                mtry = ceiling(log2(p) + 1),
                ntree = config::get("n_trees")
            )
            predictions <- rf_model$test$predicted
        } else if (algorithm == "RerF") {
            encoded_train_features <- encode_factors(train_fold$X)
            cat.map <- encoded_train_features$cat.map
            rerf_model <- RerF(
                encoded_train_features$X,
                train_fold$Y,
                task = "regression",
                mat.options = list(
                    p =
                        ifelse(is.null(cat.map), p, length(cat.map)),
                    d = ceiling(d),
                    random.matrix = "binary",
                    rho = ceiling(rho),
                    prob = 0.5
                ),
                cat.map = cat.map,
                num.cores = config::get('cores'),
                trees = config::get("n_trees")
            )
            encoded_test_features <- encode_factors(test_fold$X)
            predictions <-
                Predict(
                    encoded_test_features$X,
                    rerf_model,
                    num.cores = config::get('cores'),
                    task = 'regression'
                )
        } else {
            stop("ERROR: algorithm not recognized")
        }
        return(predictions)
    }

extract_metadata <- function(dataset) {
    dataset_mode = config::get("dataset_mode")
    if (dataset_mode == "ccfs") {
        n <- nrow(dataset)
        p_num <- sum(unlist(lapply(dataset, is.numeric)))
        p_cat <- sum(unlist(lapply(dataset, is.factor)))
    } else if (dataset_mode == "weka") {
        
    }
    return(c(
        n = n,
        p_num = p_num,
        p_cat = p_cat
    ))
}

load_local_rerf <- function() {
    base_dir = config::get("base_dir")
    if (exists("base_dir") == FALSE)
        stop("Base directory must be specified")
    devtools::install_local(base_dir)
    devtools::load_all(paste0(base_dir, "R/"))
}

get_datasets <- function() {
    data_dir = config::get("data_dir")
    if (exists("data_dir") == FALSE)
        stop("Data directory must be specified")
    
    ## Logging
    if (!is.null(config::get("logging_file"))) {
        sink(config::get("logging_file"))
    } else {
        for (i in seq_len(sink.number())) {
            sink(NULL)
        }
    }
    
    
    ## Data directory
    dataset_mode = config::get("dataset_mode")
    if (dataset_mode == "ccfs") {
        cat("Using ccfs data\n")
        datasets <- read_csv_files(data_dir)
    } else if (dataset_mode == "weka") {
        cat("Using full weka data\n")
    } else {
        stop("Dataset mode not recognized", .call = FALSE)
    }
    return(datasets)
}

results_store <- function(dataset_names) {
    base_dir = config::get("base_dir")
    rda_checkpoint_file <-
        paste0(base_dir, "results/weka_benchmarks.rda")
    csv_checkpoint_file <-
        paste0(base_dir, "results/weka_benchmarks.csv")
    if (!file.exists(rda_checkpoint_file) |
        config::get("overwrite_checkpoint")) {
        cat("Creating new checkpoint\n")
        results_matrix <- matrix(
            NA,
            nrow = length(dataset_names),
            ncol = length(config::get("result_metrics")),
            dimnames = list(dataset_names, config::get("result_metrics"))
        )
    } else {
        cat("Loading checkpoint\n")
        load(rda_checkpoint_file)
    }
    return(results_matrix)
}

read_csv_files <- function(data_dir) {
    data_files <-
        sapply(list.files(path = data_dir, pattern = "*.csv"), function(x)
            gsub(".csv", "", x), USE.NAMES = FALSE)
    csv_data <- sapply(data_files,
                       function(x) {
                           file_name <- paste0(data_dir, x, ".csv")
                           read.csv(file_name)
                       },
                       USE.NAMES = TRUE)
}

compute.rmse <-
    function(predictions, truth)
        sqrt(compute.mse)

compute.mse <-
    function(predictions, truth)
        mean((predictions - truth) ^ 2)

compute.nrmse <-
    function(predictions, truth, scaling)
        compute.rmse(predictions, truth) / scaling

compute.nmse <-
    function(predictions, truth, scaling)
        compute.mse(predictions, truth) / scaling

numeric2string <- function(x)
    format(round(x, 2), nsmall = 2)

algorithms_to_run <- function(dataset_name, results_matrix) {
    algorithms_to_benchmark = config::get("algorithms_to_benchmark")
    unfinished_algorithms =  sapply(algorithms_to_benchmark, function(x) {
        if (is.na(results_matrix[dataset_name, c(x)])) {
            return(TRUE)
        } else {
            return(FALSE)
        }
    })
    if (all(unfinished_algorithms == FALSE)) {
        return(NULL)
    } else {
        return(names(unfinished_algorithms[unfinished_algorithms == TRUE]))
    }
}
