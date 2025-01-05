#!/bin/sh
set -e

mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=release ..
cmake --build .

echo Test run:
./cmn_test
./sli_test
./nur_test
./slilin -s 1 < ../../resources/samples/slilin-0007.txt
./nurikabe -s 1 < ../../resources/samples/nurikabe-nikoli-125-5.txt

echo Build and test complete. Executables are in build directory.