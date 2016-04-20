#!/bin/bash

#Load modules on SCC
echo . ./prep_torch

#Data comes as compressed bmps. Turn it into uncompressed pngs.
cd data
echo in data/
./cleanup.sh
./genpng.sh 
cd ..


#Produce torch compatible input table.
cd process
echo in process/
./cleanup.sh
./process.sh
cd ..

#Build, train and test model.
cd recVsForExpt
echo in recVsForExpt
./runExpt.sh
cd ..