find_package(Catch2 CONFIG REQUIRED)

function(make_test name src)
    add_executable(${name} ${src} wamp_test.hpp ${PUBLIC_HEADERS})
    target_link_libraries(${name} PRIVATE Catch2::Catch2 Catch2::Catch2WithMain autobahn_cpp)
    target_include_directories(${name} PRIVATE ${BOTAN_INCLUDE_DIRS})
    string(REPLACE "test_" "" pure_name ${name})
    add_test(NAME ${name} COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/run_test.sh "${CMAKE_CURRENT_SOURCE_DIR}/${pure_name}" "${CMAKE_CURRENT_BINARY_DIR}/${name}" )
endfunction()

make_test(test_auth auth/auth.cpp)
make_test(test_pubsub pubsub/pubsub.cpp)
make_test(test_calls calls/calls.cpp)
