#!/bin/bash

# Exit on failure
set -e

# build var
: ${CXX:=g++}

# build\bin
mkdir -p bin
cd bin

# Compile
$CXX -c -o main.o ../examples/main.cpp -I../ -std=c++14

# Link
$CXX -o main main.o