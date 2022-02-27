cmake_minimum_required(VERSION 3.21)

project(autobahn-cpp)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_CXX_STANDARD 20)

option(AUTOBAHN_BUILD_EXAMPLES "Build examples" ON)
option(AUTOBAHN_TESTS "Test" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Includes/CMakeLists.txt)
if (AUTOBAHN_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif (AUTOBAHN_BUILD_EXAMPLES)

if (AUTOBAHN_TESTS)
    include(CTest)
    add_subdirectory(test)
endif()

