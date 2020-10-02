set(CMAKE_EXE_LINKER_FLAGS " -static")

add_library(examples_parameters parameters.cpp parameters.hpp)
target_include_directories(examples_parameters PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(examples_parameters PRIVATE ${Boost_LIBRARIES})

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

if(UNIX)
    make_example(uds uds.cpp)
endif()
