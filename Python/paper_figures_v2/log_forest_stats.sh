#!/usr/bin/env bash
python3 run_optimize_sklearn_classifiers.py --eval --experiment circle --clf MF > ./circle_experiment/forest_stats_MF.log
python3 run_optimize_sklearn_classifiers.py --eval --experiment circle --clf RF > ./circle_experiment/forest_stats_RF.log
python3 run_optimize_sklearn_classifiers.py --eval --experiment circle --clf SPORF > ./circle_experiment/forest_stats_SPORF.log

python3 run_optimize_sklearn_classifiers.py --eval --experiment impulse --clf MF > ./timeseries_1d/forest_stats_MF.log
python3 run_optimize_sklearn_classifiers.py --eval --experiment impulse --clf RF > ./timeseries_1d/forest_stats_RF.log
python3 run_optimize_sklearn_classifiers.py --eval --experiment impulse --clf SPORF > ./timeseries_1d/forest_stats_SPORF.log

python3 run_optimize_sklearn_classifiers.py --eval --experiment hvbar --clf MF > ./hvbar/forest_stats_MF.log
python3 run_optimize_sklearn_classifiers.py --eval --experiment hvbar --clf RF > ./hvbar/forest_stats_RF.log
python3 run_optimize_sklearn_classifiers.py --eval --experiment hvbar --clf SPORF > ./hvbar/forest_stats_SPORF.log
