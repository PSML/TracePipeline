#!/bin/bash
#Kludge
cd exp2IncPngs || exit
for i in $(ls);
do
    k=$(echo $i | sed 's/0_inc/_inc/')

    g=$(echo $i | cut -d '_' -f 4)

    mv $i $g$k
done

cd ..

cd exp3IncPngs || exit
for i in $(ls);
do
    k=$(echo $i | sed 's/0_inc/_inc/')

    g=$(echo $i | cut -d '_' -f 4)

    mv $i $g$k
done


