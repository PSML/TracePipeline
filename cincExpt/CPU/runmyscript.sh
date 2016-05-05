#!/bin/bash


pid=`qsub ./myscript.sh | cut -d ' ' -f 3`
echo "submitted job to queue"
echo "The pid is $pid and output file is myscript.sh.o$pid" 
qstat -u tommyu
touch myscript.sh.o$pid
tail -f myscript.sh.o$pid


