mkdir build
pushd build

cmake ..
cmake --build . --config release

@if %ERRORLEVEL% NEQ 0 (
  popd
  exit /b 1
)

@echo Test run:
Release\cmn_test
Release\sli_test
Release\nur_test
Release\slilin -s 1 < ..\..\resources\samples\slilin-0007.txt
Release\nurikabe -s 1 < ..\..\resources\samples\nurikabe-nikoli-125-5.txt

popd

@echo Build and test complete. Executables are in build\Release directory.