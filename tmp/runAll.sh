#!/bin/bash

#Kill any residule files.
#./cleanupAll.sh

#Data comes as compressed bmps. Turn it into uncompressed pngs.
cd data
./genExps.sh
cd ..


#Produce torch compatible input table.
for i in $( ls | grep process)
do
    cd $i
    ./process.sh
    cd ..
done

#Build, train and test model.
for i in $( ls | grep exp)
do 
    cd $i
    echo in $i
    ./runExpt.sh
    cd ..

done

