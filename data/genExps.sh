#!/bin/bash
#=====================
echo prep exp 1 data
cd exp1CompressedData
tar -vxjf exp1ForCountBmps.tar.bz2
tar -vxjf exp1RecCountBmps.tar.bz2

mkdir ../exp1ForCountPngs
mkdir ../exp1RecCountPngs

count=0
MAXJOBS=8
for i in $(ls exp1ForCountBmps)
do 
    (( $count % $MAXJOBS )) ||  wait; #Wait for all to return
    (
    mogrify -format png exp1ForCountBmps/"$i";
    mv exp1ForCountBmps/"${i%bmp}"png ../exp1ForCountPngs;
    ) &
    count=$((count+1))
done

count=0
for i in $(ls exp1RecCountBmps)
do
    (( $count % $MAXJOBS )) ||  wait; #Wait for all to return
    (
    mogrify -format png exp1RecCountBmps/"$i"
    mv exp1RecCountBmps/"${i%bmp}"png ../exp1RecCountPngs
    ) &
    count=$((count+1))
done
wait
rm -rf exp1ForCountBmps
rm -rf exp1RecCountBmps

cd ..

#=====================
echo prep exp 2 data
cd exp2CompressedData
tar -vxjf exp2IncBmps.tar.bz2
mkdir ../exp2IncPngs

count=0
for i in $(ls exp2IncBmps)
do
    (( $count % $MAXJOBS )) ||  wait; #Wait for all to return
    (
    mogrify -format png exp2IncBmps/"$i"
    mv exp2IncBmps/"${i%bmp}"png ../exp2IncPngs/
    ) &
    count=$((count+1))    
done
wait
rm -rf exp2IncBmps
cd ..

#=====================
cd exp3CompressedData
tar -vxjf exp3IncBmps.tar.bz2
mkdir ../exp3IncPngs

count=0
for i in $(ls exp3IncBmps)
do
    (( $count % $MAXJOBS )) ||  wait; #Wait for all to return
    (
    mogrify -format png exp3IncBmps/"$i"
    mv exp3IncBmps/"${i%bmp}"png ../exp3IncPngs/
    ) &
    count=$((count+1))
done
wait
rm -rf exp3IncBmps
cd ..

#=====================
./genTestLabel.sh