#!/bin/bash

rm error.log

for i in `seq 1 4`;
do
	for q in `seq 1 4`;
	do
		for j in `seq 1 2`;
		do
			./bin/fp $i $q $j
			if [ $? -ne 0 ]
			then
				echo $i $q $j >> error.log
			fi
		done
	done
done

