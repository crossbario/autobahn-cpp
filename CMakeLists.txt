# Use this for installing AutobahnCpp into <prefix>/include.
cmake_minimum_required(VERSION 2.8)

project(autobahn-cpp)

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
