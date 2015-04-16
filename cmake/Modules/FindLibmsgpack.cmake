# - Try to find libmsgpack
# Once done this will define
#  Libmsgpack_FOUND - System has libmsgpack
#  Libmsgpack_INCLUDE_DIRS - The libmsgpack include directories
#  Libmsgpack_DEFINITIONS - Compiler switches (including include directories) needed for using libmsgpack
#  Libmsgpack_LIBRARIES - The libraries needed to use libmsgpack

find_package(PkgConfig QUIET)

if(Libmsgpack_USE_STATIC_LIBS)
    set( _libmsgpack_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
endif()

if (PKG_CONFIG_FOUND)
    pkg_check_modules(PC_MSGPACK QUIET libmsgpack)
endif (PKG_CONFIG_FOUND)

find_path(Libmsgpack_INCLUDE_DIR msgpack.hpp
          HINTS ${PC_LIBMSGPACK_INCLUDEDIR} ${PC_LIBMSGPACK_INCLUDE_DIRS})

find_library(Libmsgpack_LIBRARY NAMES msgpack
             HINTS ${PC_MSGPACK_LIBDIR} ${PC_LIBMSGPACK_LIBRARY_DIRS})

if(Libmsgpack_USE_STATIC_LIBS)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${_libmsgpack_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
endif()

set(Libmsgpack_LIBRARIES ${Libmsgpack_LIBRARY})
set(Libmsgpack_INCLUDE_DIRS ${Libmsgpack_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set Libmsgpack_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Libmsgpack DEFAULT_MSG
                                  Libmsgpack_LIBRARY
                                  Libmsgpack_LIBRARIES
                                  Libmsgpack_INCLUDE_DIR
                                  Libmsgpack_INCLUDE_DIRS)

mark_as_advanced(Libmsgpack_INCLUDE_DIR Libmsgpack_LIBRARY)
