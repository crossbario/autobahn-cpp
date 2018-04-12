cmake_minimum_required(VERSION 2.8)

project(autobahn-cpp)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Includes/CMakeLists.txt)

option(AUTOBAHN_BUILD_EXAMPLES "Build examples" ON)

if(AUTOBAHN_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif(AUTOBAHN_BUILD_EXAMPLES)
