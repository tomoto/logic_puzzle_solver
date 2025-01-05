cmake_minimum_required(VERSION 3.12)
project(logic_puzzle VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

# Profile
option(ENABLE_PROFILING "Enable profiling" OFF)
if(ENABLE_PROFILING)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()

# Google Test
# https://google.github.io/googletest/quickstart-cmake.html
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/b514bdc898e2951020cbdca1304b75f5950d1f59.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Apps

file(GLOB_RECURSE CMN_SOURCES src/cmn/*.cpp src/utl/*.cpp)
file(GLOB_RECURSE SLI_SOURCES src/sli/*.cpp)
file(GLOB_RECURSE NUR_SOURCES src/nur/*.cpp)

include_directories(src)

add_executable(slilin src/app/sli/slilin.cpp ${SLI_SOURCES} ${CMN_SOURCES})
target_include_directories(slilin PRIVATE src/sli src/cmn src/utl)

add_executable(nurikabe src/app/nur/nurikabe.cpp ${NUR_SOURCES} ${CMN_SOURCES})
target_include_directories(nurikabe PRIVATE src/nur src/cmn src/utl)

# Tests

file(GLOB_RECURSE CMN_TESTS test/cmn/*_test.cpp test/utl/*_test.cpp)
file(GLOB_RECURSE SLI_TESTS test/sli/*_test.cpp)
file(GLOB_RECURSE NUR_TESTS test/nur/*_test.cpp)

add_executable(cmn_test ${CMN_TESTS} ${CMN_SOURCES})
target_include_directories(cmn_test PRIVATE src/cmn src/utl)
target_link_libraries(cmn_test gtest_main gtest)

add_executable(sli_test ${SLI_TESTS} ${SLI_SOURCES})
target_include_directories(sli_test PRIVATE src/sli src/cmn src/utl)
target_link_libraries(sli_test gtest_main gtest)

add_executable(nur_test ${NUR_TESTS} ${NUR_SOURCES})
target_include_directories(nur_test PRIVATE src/nur src/cmn src/utl)
target_link_libraries(nur_test gtest_main gtest)

enable_testing()

include(GoogleTest)
gtest_add_tests(TARGET cmn_test)
gtest_add_tests(TARGET sli_test)
gtest_add_tests(TARGET nur_test)
