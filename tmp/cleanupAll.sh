#!/bin/bash

cd data
./cleanup.sh
cd ..

for i in $( ls | grep exp)
do 
    cd $i
    ./cleanup.sh
    cd ..
done


for i in $( ls | grep exp)
do 
    cd $i
    ./cleanup.sh
    cd ..
done
