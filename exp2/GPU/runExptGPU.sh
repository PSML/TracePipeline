#!/bin/bash

echo Generating network.
th genNetGPU.lua
echo Training network.
th trainNetGPU.lua
echo Testing network.
th testNetGPU.lua
