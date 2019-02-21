#!/bin/bash

if [ -f error.log ]; then
	rm error.log
fi

for i in `seq 1 8`;
do
	for q in 1 2 3 5 6;
	do
		./bin/fp $i $q 1
		if [ $? -ne 0 ]
		then
			echo $i $q $j >> error.log
		fi
	done
done

if [ -f error.log ]; then
	echo "errors were found"	
else
	echo "All tests ran successfully"	
fi


