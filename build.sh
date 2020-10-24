#!/bin/bash

cd build
export CC=clang

if command -v cmake3 &> /dev/null
then
   cmakecmd=cmake3
else
   cmakecmd=cmake
fi

$cmakecmd .. && make
