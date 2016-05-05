#!/bin/bash
#=====================
echo prep exp 1 data
cd exp1CompressedData
tar -vxjf exp1ForCountBmps.tar.bz2
tar -vxjf exp1RecCountBmps.tar.bz2

mogrify -format png exp1ForCountBmps/*
mogrify -format png exp1RecCountBmps/*

mkdir ../exp1ForCountPngs
mkdir ../exp1RecCountPngs

mv exp1ForCountBmps/*.png ../exp1ForCountPngs
mv exp1RecCountBmps/*.png ../exp1RecCountPngs

rm -rf exp1ForCountBmps
rm -rf exp1RecCountBmps
cd ..

#=====================
echo prep exp 2 data
cd exp2CompressedData
tar -vxjf exp2IncBmps.tar.bz2

mogrify -format png exp2IncBmps/*

mkdir ../exp2IncPngs
mv exp2IncBmps/*.png ../exp2IncPngs/
rm -rf exp2IncBmps
cd ..

#=====================
cd exp3CompressedData

tar -vxjf exp3IncBmps.tar.bz2

mogrify -format png exp3IncBmps/*

mkdir ../exp3IncPngs
mv exp3IncBmps/*.png ../exp3IncPngs/

rm -rf exp3IncBmps
cd ..

#=====================
./genTestLabel.sh