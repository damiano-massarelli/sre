#!/usr/bin/env bash

BUILD_DIR="vsbuild"

if [ -d $BUILD_DIR ]; then
    rm -r $BUILD_DIR
fi

mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake -G "Visual Studio 15 Win64" ..
#cmake --build .
