#!/usr/bin/env bash

mkdir -p Debug Release
cd Debug
cmake -DCMAKE_BUILD_TYPE=Debug ../../
cd ../Release
cmake -DCMAKE_BUILD_TYPE=Release ../../
