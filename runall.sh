#!/bin/bash

#Load modules on SCC
. ./prep_torch

#Data comes as compressed bmps. Turn it into uncompressed pngs.
cd data
./genpng.sh 
cd ..

cd process
echo "th process.lua: NYI"
echo "turns 2 directories of pngs into a tensor."
cd ..

cd recVsForExpt
./runexpt.sh
cd ..