find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

set(Boost_NO_WARN_NEW_VERSIONS ON)
find_package(Boost REQUIRED COMPONENTS program_options filesystem system thread random)

find_package(msgpack CONFIG REQUIRED)
find_package(websocketpp CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(BOTAN2 botan-2 REQUIRED)

MESSAGE(STATUS "AUTOBAHN_BUILD_EXAMPLES:  " ${AUTOBAHN_BUILD_EXAMPLES})
MESSAGE(STATUS "AUTOBAHN_USE_LIBCXX:      " ${AUTOBAHN_USE_LIBCXX})
MESSAGE(STATUS "CMAKE_ROOT:               " ${CMAKE_ROOT})
MESSAGE(STATUS "CMAKE_INSTALL_PREFIX:     " ${CMAKE_INSTALL_PREFIX})
MESSAGE(STATUS "Boost_INCLUDE_DIRS:       " ${Boost_INCLUDE_DIRS})
MESSAGE(STATUS "Boost_LIBRARIES:          " ${Boost_LIBRARIES})
get_target_property(MSGPACKC_INCLUDE_DIR msgpackc INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(MSGPACKC_LIBRARIES msgpackc IMPORTED_LOCATION_RELEASE)
MESSAGE(STATUS "MSGPACKC_INCLUDE_DIR:     " ${MSGPACKC_INCLUDE_DIR})
MESSAGE(STATUS "MSGPACKC_LIBRARIES:        " ${MSGPACKC_LIBRARIES})
MESSAGE(STATUS "WEBSOCKETPP_INCLUDE_DIR: " ${WEBSOCKETPP_INCLUDE_DIR})

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
        ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_publish_options.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_publish_options.ipp
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
        ${OPENSSL_INCLUDE_DIR}
        ${BOTAN2_INCLUDE_DIRS}
        ${WEBSOCKETPP_INCLUDE_DIR}
        ${MSGPACKC_INCLUDE_DIR})

target_link_libraries(autobahn_cpp INTERFACE
        Boost::boost Boost::thread
        websocketpp::websocketpp
        msgpackc msgpackc-cxx
        OpenSSL::SSL OpenSSL::Crypto
        ${BOTAN2_LINK_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${CMAKE_DL_LIBS})

foreach(h ${PUBLIC_HEADERS})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "include/" include "${h}")
    get_filename_component(HEADER_INCLUDE_DIRECTORY ${include} DIRECTORY)
    install(FILES ${h}
        DESTINATION ${HEADER_INCLUDE_DIRECTORY}
        COMPONENT "headers")
endforeach()
