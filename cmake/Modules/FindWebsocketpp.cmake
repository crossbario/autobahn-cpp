# - Try to find websocketpp
# Once done this will define
#  Websocketpp_FOUND - System has websocketpp
#  Websocketpp_INCLUDE_DIRS - The websocketpp include directories

set(_env "$ENV{WEBSOCKETPP_ROOT}")
if(_env)

    set(Websocketpp_FOUND TRUE)
    set(Websocketpp_INCLUDE_DIRS "$ENV{WEBSOCKETPP_ROOT}/include")
    set(Websocketpp_LIBRARIES "$ENV{WEBSOCKETPP_ROOT}/libs")

elseif(WEBSOCKETPP_ROOT)

    set(Websocketpp_FOUND TRUE)
    set(Websocketpp_INCLUDE_DIRS "${WEBSOCKETPP_ROOT}/include")
    set(Websocketpp_LIBRARIES "${WEBSOCKETPP_ROOT}/libs")

else()

    find_package(PkgConfig QUIET)

    if (PKG_CONFIG_FOUND)
        pkg_check_modules(PC_WEBSOCKETPP QUIET websocketpp)
    endif (PKG_CONFIG_FOUND)

    find_path(Websocketpp_INCLUDE_DIR websocketpp
              HINTS ${PC_WEBSOCKETPP_INCLUDEDIR} ${PC_WEBSOCKETPP_INCLUDE_DIRS})

    set(Websocketpp_INCLUDE_DIRS ${Websocketpp_INCLUDE_DIR})

    include(FindPackageHandleStandardArgs)
    # handle the QUIETLY and REQUIRED arguments and set Websocketpp_FOUND to TRUE
    # if all listed variables are TRUE
    find_package_handle_standard_args(websocketpp DEFAULT_MSG
                                      Websocketpp_INCLUDE_DIR
                                      Websocketpp_INCLUDE_DIRS)

    mark_as_advanced(Websocketpp_INCLUDE_DIR)

endif()
