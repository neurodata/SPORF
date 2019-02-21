#!/bin/bash

if [ -f error.log ]; then
	rm error.log
fi

for i in `seq 1 8`;
do
	./bin/fp $i 1 1 > garbage.out
	if [ $? -ne 0 ]
	then
		echo $i >> error.log
	fi
	printf "\n"
done

rm garbage.out

if [ -f error.log ]; then
	echo "errors were found"	
else
	./res/showRes.sh
fi



