#!/bin/bash

echo Generating network.
th genNet.lua
echo Training network.
th trainNet.lua
echo Testing network.
th testNet.lua
