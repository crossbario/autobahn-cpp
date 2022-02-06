if (!APPLE)
    set(CMAKE_EXE_LINKER_FLAGS " -static")
endif()

add_library(examples_parameters parameters.cpp parameters.hpp)
target_include_directories(examples_parameters PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(examples_parameters PRIVATE ${Boost_LIBRARIES})
target_compile_definitions(examples_parameters PRIVATE ${Boost_DEFINITIONS})

function(make_example name src)
    add_executable(${name} ${src} ${PUBLIC_HEADERS})
    target_link_libraries(${name} examples_parameters autobahn_cpp)
endfunction()

make_example(caller caller.cpp)
make_example(callee callee.cpp)
make_example(provide_prefix provide_prefix.cpp)
make_example(publisher publisher.cpp)
make_example(subscriber subscriber.cpp)
make_example(wampcra wampcra.cpp)
make_example(websocket_callee websocket_callee.cpp)
make_example(cryptosign-openssl cryptosign-openssl.cpp)
if (AUTOBAHN_BUILD_EXAMPLES_BOTAN)
    find_package(Botan2 REQUIRED)
    make_example(cryptosign-botan cryptosign-botan.cpp)
    target_include_directories(cryptosign-botan PRIVATE ${BOTAN_INCLUDE_DIRS})
    target_link_libraries(cryptosign-botan Botan2::Botan2)
endif()

if(UNIX)
    make_example(uds uds.cpp)
endif()

# By default MSVC has a 2^16 limit on the number of sections in an object file,
# and this needs more than that.
if (MSVC)
    set_source_files_properties(websocket_callee.cpp PROPERTIES COMPILE_FLAGS /bigobj)
endif()
