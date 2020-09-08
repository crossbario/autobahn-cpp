set(CMAKE_EXE_LINKER_FLAGS " -static")

add_library(examples_parameters parameters.cpp parameters.hpp)

function(make_example name src)
    add_executable(${name} ${src} ${PUBLIC_HEADERS})
    target_link_libraries(${name} examples_parameters autobahn_cpp ${ARGN})
endfunction()

make_example(caller caller.cpp)
make_example(callee callee.cpp)
make_example(provide_prefix provide_prefix.cpp)
make_example(publisher publisher.cpp)
make_example(subscriber subscriber.cpp)
make_example(wampcra wampcra.cpp crypto)
make_example(websocket_callee websocket_callee.cpp)

if(UNIX)
    make_example(uds uds.cpp)
endif()
