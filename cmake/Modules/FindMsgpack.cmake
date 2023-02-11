# - Try to find msgpack
# Once done this will define
#  Msgpack_FOUND - System has msgpack
#  Msgpack_INCLUDE_DIRS - The msgpack include directories

set(_env "$ENV{MSGPACK_ROOT}")
if(_env)

    set(Msgpack_FOUND TRUE)
    set(Msgpack_INCLUDE_DIRS "$ENV{MSGPACK_ROOT}/include")
    set(Msgpack_LIBRARIES "$ENV{MSGPACK_ROOT}/libs")

elseif(MSGPACK_ROOT)

    set(Msgpack_FOUND TRUE)
    set(Msgpack_INCLUDE_DIRS "${MSGPACK_ROOT}/include")
    set(Msgpack_LIBRARIES "${MSGPACK_ROOT}/libs")

else()

    find_package(PkgConfig QUIET)

    if (PKG_CONFIG_FOUND)
        pkg_check_modules(PC_MSGPACK QUIET msgpack)
    endif (PKG_CONFIG_FOUND)

    find_path(Msgpack_INCLUDE_DIR msgpack.hpp
              HINTS ${PC_MSGPACK_INCLUDEDIR} ${PC_MSGPACK_INCLUDE_DIRS})

    set(Msgpack_INCLUDE_DIRS ${Msgpack_INCLUDE_DIR})

    include(FindPackageHandleStandardArgs)
    # handle the QUIETLY and REQUIRED arguments and set Msgpack_FOUND to TRUE
    # if all listed variables are TRUE
    find_package_handle_standard_args(msgpack DEFAULT_MSG
                                      Msgpack_INCLUDE_DIR
                                      Msgpack_INCLUDE_DIRS)

    mark_as_advanced(Msgpack_INCLUDE_DIR)

endif()
