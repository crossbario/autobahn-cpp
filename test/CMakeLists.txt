Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.0.0-preview3
)
FetchContent_MakeAvailable(Catch2)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.0.0-preview3
)
find_package(Botan2 REQUIRED)
function(make_test name src)
    add_executable(${name} ${src} wamp_test.hpp ${PUBLIC_HEADERS})
    target_link_libraries(${name} PRIVATE Catch2::Catch2WithMain autobahn_cpp Botan2::Botan2)
    target_include_directories(${name} PRIVATE ${BOTAN_INCLUDE_DIRS})
    string(REPLACE "test_" "" pure_name ${name})
    add_test(NAME ${name} COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/run_test.sh "${CMAKE_CURRENT_SOURCE_DIR}/${pure_name}" "${CMAKE_CURRENT_BINARY_DIR}/${name}" )
endfunction()

make_test(test_auth auth/auth.cpp)
make_test(test_pubsub pubsub/pubsub.cpp)
make_test(test_calls calls/calls.cpp)
