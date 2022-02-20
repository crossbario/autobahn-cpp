cmake_minimum_required(VERSION 3.21)

set(vcpkg "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
if (NOT CMAKE_TOOLCHAIN_FILE AND EXISTS "${vcpkg}")
    set(CMAKE_TOOLCHAIN_FILE "${vcpkg}"
            CACHE FILEPATH "CMake toolchain file")
    message(STATUS "vcpkg toolchain found: ${CMAKE_TOOLCHAIN_FILE}")
endif ()

project(autobahn-cpp)

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

