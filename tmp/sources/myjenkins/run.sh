#How many chars to hash.
G=20
NUM=10
./cleanup.sh
mkdir intermed
mkdir images

echo Compiling and building memory images for jenkins hash on random alphanumeric stings.
echo this is a kludge and should be in a makefile
echo Creating $NUM images using hashes of $G chars.


for i in $(seq 1 $NUM)
do
    K=$(cat /dev/urandom | gtr -cd 'A-Z0-9' | head -c $G)
    ../../../../ext/install/bin/cc65 -D SIZE=$G -D KEY=\"${K}\" -D__6502__ -t none -O -Oi --cpu 6502 jenkins.c -o intermed/jenkins_${K}.s
    ../../../../ext/install/bin/ca65 --cpu 6502 intermed/jenkins_${K}.s -l intermed/jenkins_${K}.lst
    ../../../../ext/install/bin/ld65 -o intermed/jenkins_${K} -C ../bu6502.cfg  intermed/jenkins_${K}.o  ../bu6502.lib   -m intermed/jenkins_${K}.map

    cp intermed/jenkins_${K} images/jenkins_${K}.img
done

echo Done compiling. 
echo images are in ./images, intermediate files in ./intermed

echo about to process images

cd process
./run.sh