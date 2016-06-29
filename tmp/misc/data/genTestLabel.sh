#!/bin/bash
#Kludge
MAXJOBS=8
echo Prepending correct training labels.

cd exp2IncPngs || exit
count=0

for i in $(ls);
do
    (( $count % $MAXJOBS )) ||  wait; #Wait for all to return
    (
    k=$(echo $i | sed 's/0_inc/_inc/')

    g=$(echo $i | cut -d '_' -f 4)

    mv $i $g$k
    ) &
    count=$((count+1))
done

cd ..

cd exp3IncPngs || exit
count=0
for i in $(ls);
do
    (( $count % $MAXJOBS )) ||  wait; #Wait for all to return
    (
    k=$(echo $i | sed 's/0_inc/_inc/')

    g=$(echo $i | cut -d '_' -f 4)

    mv $i $g$k
    ) &
    count=$((count+1))
done


