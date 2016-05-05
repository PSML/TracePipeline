#!/bin/bash
# A way too complex script to submit jobs to scc 

pid=`qsub -P psml ./myscript.sh | cut -d ' ' -f 3`
echo "submitted job to queue"
echo "The pid is $pid and output file is myscript.sh.o$pid" 
status=`qstat -u $USER | grep $pid | awk '{print $5}'`
echo $status

while [ "$status" != "r" ]; do
	qstat -u $USER
	sleep 1
	status=`qstat -u $USER | grep $pid | awk '{print $5}'`
done

tail -f myscript.sh.o$pid
