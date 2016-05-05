#!/bin/bash

#$ -j y
#$ -pe omp 16
#$ -V
th trainNet.lua
