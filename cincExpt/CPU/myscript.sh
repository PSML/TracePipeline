#!/bin/bash

#$ -pe omp 16
#$ -V
th trainNet.lua
