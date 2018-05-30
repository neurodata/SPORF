# Table writing 
base_dir <- config::get("base_dir")
csv_file <-
  paste0(base_dir, "results/regression_benchmark_full_run_2.csv")
table_frame <- read.csv(csv_file)
table_frame <- table_frame[,-7]
names(table_frame)[1] <- "Datasets"
x <- xtable(table_frame, caption = "temp", tabular.environment = "longtable", latex.environments = "center", include.rownames=F )
align(x) <- "rr|lll|lll"
print(x, include.rownames=FALSE)