#!/bin/bash

#Load modules on SCC
. ./prep_torch

#Data comes as compressed bmps. Turn it into uncompressed pngs.
cd data
./cleanup.sh
./genpng.sh 
cd ..

#Produce torch compatible input table.
cd process
./process.sh
cd ..

#Build, train and test model.
cd recVsForExpt
./runexpt.sh
cd ..