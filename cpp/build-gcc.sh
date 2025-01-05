#!/bin/sh
set -e

mkdir -p build
cd build


os="$(uname -s)"
if [ "$os" = "Darwin" ]; then
  # For macOS, use Homebrew-installed GCC
  cmake -DCMAKE_C_COMPILER=gcc-14 -DCMAKE_CXX_COMPILER=g++-14 -DCMAKE_BUILD_TYPE=Release ..
else
  # For Linux such as Ubuntu on WSL2
  cmake -DCMAKE_BUILD_TYPE=Release ..
fi

cmake --build .

echo Test run:
./cmn_test
./sli_test
./nur_test
./slilin -s 1 < ../../resources/samples/slilin-0007.txt
./nurikabe -s 1 < ../../resources/samples/nurikabe-nikoli-125-5.txt

echo Build and test complete. Executables are in build directory.