#!/bin/bash

#Kill any residule files.
./cleanupAll.sh

#Load modules on SCC
#How to do this without launching subshell?
#. ./prep_torch

#Data comes as compressed bmps. Turn it into uncompressed pngs.
cd data
echo in data/
./genpng.sh 
cd ..


#Produce torch compatible input table.
cd process
echo in process/
./process.sh
cd ..

#Build, train and test model.
cd recVsForExpt
echo in recVsForExpt
./runExpt.sh
cd ..

