# Use this for installing AutobahnCpp into <prefix>/include.
cmake_minimum_required(VERSION 2.8)

project(autobahn-cpp)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror ${CMAKE_CXX_FLAGS}")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-stdlib=libc++ ${CMAKE_CXX_FLAGS}")
    if ("${CMAKE_GENERATOR}" STREQUAL "Ninja")
        set(CMAKE_CXX_FLAGS "-fcolor-diagnostics ${CMAKE_CXX_FLAGS}")
    endif()
endif()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(Boost REQUIRED COMPONENTS system thread)
find_package(Libmsgpack REQUIRED)
find_package(Threads REQUIRED)

set(PUBLIC_HEADERS
    autobahn/autobahn.hpp
    autobahn/exceptions.hpp
    autobahn/wamp_call.hpp
    autobahn/wamp_call.ipp
    autobahn/wamp_endpoints.hpp
    autobahn/wamp_message.hpp
    autobahn/wamp_message_type.hpp
    autobahn/wamp_publication.hpp
    autobahn/wamp_publication.ipp
    autobahn/wamp_register_request.hpp
    autobahn/wamp_register_request.ipp
    autobahn/wamp_registration.hpp
    autobahn/wamp_registration.ipp
    autobahn/wamp_session.hpp
    autobahn/wamp_session.ipp
    autobahn/wamp_subscribe_request.hpp
    autobahn/wamp_subscribe_request.ipp
    autobahn/wamp_subscription.hpp
    autobahn/wamp_subscription.ipp)

foreach(h ${PUBLIC_HEADERS})
    get_filename_component(HEADER_INCLUDE_DIRECTORY include/${h} DIRECTORY)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${h}
        DESTINATION ${HEADER_INCLUDE_DIRECTORY}
        COMPONENT "headers")
endforeach()

add_subdirectory(examples)
