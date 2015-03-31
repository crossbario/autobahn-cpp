# Use this for installing AutobahnCpp into <prefix>/include.
cmake_minimum_required(VERSION 2.8)

project(autobahn-cpp)

set(PUBLIC_HEADERS
    autobahn/autobahn.hpp
    autobahn/autobahn_impl.hpp)

foreach(h ${PUBLIC_HEADERS})
    get_filename_component(HEADER_INCLUDE_DIRECTORY include/${h} DIRECTORY)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${h}
        DESTINATION ${HEADER_INCLUDE_DIRECTORY}
        COMPONENT "headers")
endforeach()
