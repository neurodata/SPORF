#!/bin/bash

rm error.log

for i in `seq 1 4`;
do
		./bin/fp $i 1 1
		if [ $? -ne 0 ]
		then
			echo $i $q $j >> error.log
		fi
		printf "\n"
done

