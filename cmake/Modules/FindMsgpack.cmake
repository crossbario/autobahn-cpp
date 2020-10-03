# - Try to find msgpack
# Once done this will define
#  msgpack_FOUND - System has msgpack
#  msgpack_INCLUDE_DIRS - The msgpack include directories

set(_env "$ENV{MSGPACK_ROOT}")
if(_env)

    set(msgpack_FOUND TRUE)
    set(msgpack_INCLUDE_DIRS "$ENV{MSGPACK_ROOT}/include")
    set(msgpack_LIBRARIES "$ENV{MSGPACK_ROOT}/libs")

else()

    find_package(PkgConfig QUIET)

    if (PKG_CONFIG_FOUND)
        pkg_check_modules(PC_MSGPACK QUIET msgpack)
    endif (PKG_CONFIG_FOUND)

    find_path(msgpack_INCLUDE_DIR msgpack.hpp
              HINTS ${PC_MSGPACK_INCLUDEDIR} ${PC_MSGPACK_INCLUDE_DIRS})

    set(msgpack_INCLUDE_DIRS ${msgpack_INCLUDE_DIR})

    include(FindPackageHandleStandardArgs)
    # handle the QUIETLY and REQUIRED arguments and set Msgpack_FOUND to TRUE
    # if all listed variables are TRUE
    find_package_handle_standard_args(msgpack DEFAULT_MSG
                                      msgpack_INCLUDE_DIR
                                      msgpack_INCLUDE_DIRS)

    mark_as_advanced(msgpack_INCLUDE_DIR)

endif()
