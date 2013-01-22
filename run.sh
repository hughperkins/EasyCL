#!/bin/bash

target=$1

rsync --exclude=build --exclude=.git -av ./ -e "ssh -p 3000" hughperkins@localhost:/home/hughperkins/dev/OpenCLHelper/

#ssh -p 3000 hughperkins@localhost cd /home/hughperkins/dev/OpenCLHelper \; g++ -fopenmp -g -O2 -o $target -I . $target.cpp OpenCLHelper.cpp CLKernel.cpp clcc/clew.c -ldl -lrt \&\& time ./$target

ssh -p 3000 hughperkins@localhost cd /home/hughperkins/dev/OpenCLHelper/build \&\& cmake .. \&\& make $target \&\& time ./$target
    
