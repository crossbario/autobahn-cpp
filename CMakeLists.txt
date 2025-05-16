cmake_minimum_required(VERSION 3.21)

project(autobahn-cpp)

if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/develop/conan.cmake"
            "${CMAKE_BINARY_DIR}/conan.cmake"
            TLS_VERIFY ON)
endif ()

file(COPY ${CMAKE_SOURCE_DIR}/conanfile.py DESTINATION ${CMAKE_BINARY_DIR})

include(${CMAKE_BINARY_DIR}/conan.cmake)

set(CMAKE_CXX_STANDARD 17)

option(AUTOBAHN_BUILD_EXAMPLES "Build examples" ON)
option(AUTOBAHN_TESTS "Test" ON)

conan_cmake_run(CONANFILE ${CMAKE_SOURCE_DIR}/conanfile.py
        BASIC_SETUP CMAKE_TARGETS
        BUILD missing)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
        BUILD missing
        REMOTE conancenter
        SETTINGS ${settings})

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Includes/CMakeLists.txt)
if (AUTOBAHN_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif (AUTOBAHN_BUILD_EXAMPLES)

if (AUTOBAHN_TESTS)
    include(CTest)
    add_subdirectory(test)
endif ()
