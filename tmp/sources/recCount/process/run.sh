rm -rf traces
rm -rf bmps
mkdir traces
mkdir bmps

echo generating traces

for i in $( ls ../images )
do

    j=$( echo $i | sed -e 's/img/trc/g')
    echo $j
    #this is error prone, no good way to get #rows.
    ../../../../../6502 -t sv -o traces/$j ../images/$i /dev/null 2> tmp

done

#Totally going to break in the future
ROWS=$(cat tmp | grep i= | cut -d ' ' -f 3 | tr -d "i=")
echo num rows is $ROWS
rm tmp

#this is a kludge because 2bmp breaks if you try to use it a level up
cd traces 

echo slicing out register state, generating a bmp for each trace

for i in $( ls )
do
    ../../../../../../2bmp -e 0,7 -r $ROWS 65544 $i
done

mv *.bmp ../bmps
cd ..


echo about to enter octave code to process bmps into one composite image
echo which should show the regions that change dependant on input data.
echo this is both the input data, and any derivatives. 

octave genDataDep

echo component traces can be found in ./process/bmps/
echo you can view this composite image in ./process/data_dep.bmp

echo you should confirm youre getting the expected num rows.
