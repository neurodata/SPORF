rm(list = ls())
debugSource("utils.R")

Sys.setenv(R_CONFIG_ACTIVE = "local")

# Perform preliminary set up
load_local_rerf()
all_datasets <- get_datasets()
dataset_names <- names(all_datasets)
results_matrix <- results_store(dataset_names)
verbose <- config::get("verbose")

# Run for each dataset
# set.seed(131)
for (dataset_name in sort(dataset_names)) {
    # Perform any preprocessing
    dataset <- preprocess(all_datasets[[dataset_name]])
    if (nrow(dataset) < 10) {
        cat(sprintf("%s has insufficient full samples", dataset_name),
            '\n')
        next
    }
    metadata <- extract_metadata(dataset)
    results_matrix[dataset_name, c('n', 'p_num', 'p_cat')] <-
        metadata
    # if (metadata['p_cat'] > 0) next
    
    # Get algorithms to benchmark
    unfinished_algorithms <-
        algorithms_to_run(dataset_name, results_matrix)
    if (is.null(unfinished_algorithms)) {
        cat(sprintf("%s finished being ran", dataset_name), '\n')
        next
    } else {
        cat(sprintf("Benchmarking %s", dataset_name), '\n')        
    }

    
    # Train epochs
    epochs <- config::get("epochs")
    folds_k <- config::get("k_folds")
    epoch_mean <- sapply(unfinished_algorithms,
                         function(x)
                             numeric(epochs),
                         USE.NAMES = TRUE, simplify=FALSE)
    epoch_sd <- sapply(unfinished_algorithms,
                       function(x)
                           numeric(epochs),
                       USE.NAMES = TRUE, simplify=FALSE)
    for (i in 1L:epochs) {
        folds_i <- sample(rep(1L:folds_k, length.out = metadata['n']))
        # Train k-fold validation for each algorithm
        for (algorithm in unfinished_algorithms) {
            if (algorithm == 'RerF') {
                cv_results <- NULL
                best_d <- NULL
                best_rho <- NULL
                if (any(sapply(dataset, is.factor)) == TRUE) {
                    p <- sum(as.integer(sapply(dataset, is.factor)))
                } else {
                    p <- dim(dataset)[2]
                }
                for (d in sapply(c(p^0.25, p^0.33, p^0.5, p, p^2), ceiling)) {
                    for (rho in sapply(c(1/p, 2/p, 3/p, 4/p), ceiling)) {
                        if (round(p*d*rho) <= (p*d)) {
                            curr_results <- get_cv_results(algorithm, dataset, folds_i, d, rho)
                            if (is.null(cv_results) | (mean(curr_results) < mean(cv_results))) {
                                cv_results <- curr_results
                                best_d <- d
                                best_rho <- rho
                            }
                        }
                    }
                }
            } else {
                cv_results <- get_cv_results(algorithm, dataset, folds_i)
            }
            epoch_mean[[algorithm]][i] <- mean(cv_results)
            epoch_sd[[algorithm]][i] <- sd(cv_results)
            if (algorithm == "RerF") {
                progress_string <- sprintf(
                    "(Dataset) %s \t (Algorithm) %s \t d=%f, rho=%f \t on epoch %i: %s ",
                    dataset_name,
                    algorithm,
                    best_d,
                    best_rho,
                    i,
                    numeric2string(epoch_mean[[algorithm]][i])
                )
            } else {
                progress_string <- sprintf(
                    "(Dataset) %s \t (Algorithm) %s \t on epoch %i: %s ",
                    dataset_name,
                    algorithm,
                    i,
                    numeric2string(epoch_mean[[algorithm]][i])
                )
            }
            cat(progress_string, '\n\n')
        }
    }
    aggregate_results <- sapply(unfinished_algorithms,
                                function(algorithm) {
                                    if (epochs == 1L) {
                                        return(final_result_string(epoch_mean[[algorithm]],
                                                                   epoch_sd[[algorithm]]))
                                    } else {
                                        return(final_result_string(mean(epoch_mean[[algorithm]]),
                                                                   sd(epoch_mean[[algorithm]])))
                                    }
                                },
                                USE.NAMES = TRUE)
    for (algorithm in unfinished_algorithms) {
        cat(
            sprintf(
                "(Dataset) %s \t (Algorithm) %s \t Aggregate %s ",
                dataset_name,
                algorithm,
                aggregate_results[[algorithm]]
            ),
            '\n\n'
        )
        results_matrix[dataset_name, algorithm] <-
            aggregate_results[[algorithm]]
    }
    # Save checkpoint
    save_checkpoint(results_matrix)
}
run_signed_rank_test(results_matrix, "RerF", "RF")