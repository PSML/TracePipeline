#!/bin/bash

cd compressedDataCINC
tar -vxjf incBmps3.tar.bz2

mogrify -format png cBmps3/*

mkdir ../cINCpngs
#Think this is broken
mv cBmps3/*.png ../cINCpngs

#rm -rf cINCbmps

exit

cd ../compressedDataINC
tar -vxjf incBmps.tar.bz2

mogrify -format png bmps/*

mkdir ../incCountpngs
mv bmps/*.png ../incCountpngs
rm -rf bmps