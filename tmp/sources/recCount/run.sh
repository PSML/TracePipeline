#!/bin/bash
#Num digits for start pos.
DIGS=4
#Num of repititions.
REPS=10

./cleanup.sh
mkdir intermed
mkdir images

echo Building recursive counting programs. Starting at random offsets and counting to ten in increments of 1.
echo this is a kludge and should be in a makefile


for i in $(seq 1 $REPS)
do
    #Grab a random $DIGS digit num. No zeros to avoid non nums. 
    K=$(cat /dev/urandom | gtr -cd '0-9' | head -c $DIGS)
    K=$(echo $K | sed 's/^0*//')
    echo start at $K 
    #Compile
    #With optimization
#    ../../../../ext/install/bin/cc65 -D START=$K -D MAX=$[K+10] -D__6502__ -t none -O -Oi --cpu 6502 recCount.c -o intermed/recCount_${K}.s
    #No optimization
    ../../../../ext/install/bin/cc65 -D START=$K -D MAX=$[K+10] -D__6502__ -t none --cpu 6502 recCount.c -o intermed/recCount_${K}.s
    ../../../../ext/install/bin/ca65 --cpu 6502 intermed/recCount_${K}.s -l intermed/recCount_${K}.lst
    ../../../../ext/install/bin/ld65 -o intermed/recCount_${K} -C ../bu6502.cfg  intermed/recCount_${K}.o  ../bu6502.lib   -m intermed/recCount_${K}.map
    cp intermed/recCount_${K} images/recCount_${K}.img
done

echo Done compiling. 
echo images are in ./images, intermediate files in ./intermed

echo about to process images

cd process

./run.sh