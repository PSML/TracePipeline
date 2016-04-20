#!/bin/bash

tar -vxjf recCountbmps.tar.bz2
tar -vxjf forCountbmps.tar.bz2

mogrify -format png recCountbmps/*
mogrify -format png forcountbmps/*

mkdir recCountpngs
mkdir forCountpngs

mv recCountbmps/*.png recCountpngs
mv forcountbmps/*.png forCountpngs

rm -rf recCountbmps
rm -rf forcountbmps
