cmake_minimum_required(VERSION 2.8)

project(autobahn-cpp)

option(AUTOBAHN_BUILD_EXAMPLES "Build examples" ON)
option(AUTOBAHN_BUILD_EXAMPLES_BOTAN "Build Botan cryptosign example" OFF)
option(AUTOBAHN_TESTS "Test" ON)
option(AUTOBAHN_USE_LIBCXX "Use libc++ instead of libstdc++ when building with Clang" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Includes/CMakeLists.txt)

if(AUTOBAHN_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif(AUTOBAHN_BUILD_EXAMPLES)

if (AUTOBAHN_TESTS)
    include(CTest)
    add_subdirectory(test)
endif()