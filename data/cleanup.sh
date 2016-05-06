#!/bin/bash
rm -rf exp1ForCountPngs
rm -rf exp1RecCountPngs
rm -rf exp2IncPngs
rm -rf exp3IncPngs
rm *~

cd exp1CompressedData
rm -rf exp1ForCountBmps
rm -rf exp1RecCountBmps
cd ..

cd exp2CompressedData
rm -rf exp2IncBmps
cd ..

cd exp3CompressedData
rm -rf exp3IncBmps
cd ..