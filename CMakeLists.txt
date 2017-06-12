# Use this for installing AutobahnCpp into <prefix>/include.
cmake_minimum_required(VERSION 2.8)

project(autobahn-cpp)

option(AUTOBAHN_BUILD_EXAMPLES "Build examples" ON)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-stdlib=libc++ -Wno-unused-local-typedefs -Wno-unknown-warning-option ${CMAKE_CXX_FLAGS}")
    if ("${CMAKE_GENERATOR}" STREQUAL "Ninja")
        set(CMAKE_CXX_FLAGS "-fcolor-diagnostics ${CMAKE_CXX_FLAGS}")
    endif()
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9.0") # i.e. if >= 4.9.0
        if (CMAKE_GENERATOR STREQUAL "Ninja")
            set(CMAKE_CXX_FLAGS "-fdiagnostics-color=always ${CMAKE_CXX_FLAGS}")
        else()
            set(CMAKE_CXX_FLAGS "-fdiagnostics-color=auto ${CMAKE_CXX_FLAGS}")
        endif()
    endif()
endif()

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -Wno-unused-variable ${CMAKE_CXX_FLAGS}")
endif()

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

find_package(Boost REQUIRED COMPONENTS program_options system thread random)
find_package(Websocketpp REQUIRED)
find_package(Msgpack REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

set(PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/autobahn.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/boost_config.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/exceptions.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_arguments.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_auth_utils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_authenticate.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_authenticate.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_call.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_call.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_call_options.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_call_options.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_call_result.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_call_result.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_challenge.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_challenge.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_event.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_event.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_event_handler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_invocation.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_invocation.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_message.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_message.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_message_type.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_message_type.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_procedure.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_publication.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_publication.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_rawsocket_transport.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_rawsocket_transport.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_register_request.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_register_request.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_registration.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_registration.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_transport.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_transport_handler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_session.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_session.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_subscribe_options.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_subscribe_options.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_subscribe_request.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_subscribe_request.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_subscription.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_subscription.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_tcp_transport.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_tcp_transport.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_transport_handler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_transport.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_uds_transport.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_unregister_request.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_unregister_request.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_unsubscribe_request.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_unsubscribe_request.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_websocket_transport.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_websocket_transport.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_websocketpp_websocket_transport.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_websocketpp_websocket_transport.ipp
    )

add_library(autobahn_cpp INTERFACE)
target_include_directories(autobahn_cpp INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${Websocketpp_INCLUDE_DIRS}
    ${Msgpack_INCLUDE_DIRS})
target_link_libraries(autobahn_cpp INTERFACE
    ${Boost_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT})

foreach(h ${PUBLIC_HEADERS})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "include/" include "${h}")
    get_filename_component(HEADER_INCLUDE_DIRECTORY ${include} DIRECTORY)
    install(FILES ${h}
        DESTINATION ${HEADER_INCLUDE_DIRECTORY}
        COMPONENT "headers")
endforeach()

if(AUTOBAHN_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif(AUTOBAHN_BUILD_EXAMPLES)
