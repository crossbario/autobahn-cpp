Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.0.0-preview3
)
FetchContent_MakeAvailable(Catch2)

function(make_test name src)
    # By default MSVC has a 2^16 limit on the number of sections in an object file,
    # and this needs more than that.
    if (MSVC)
        set_source_files_properties(${src} PROPERTIES COMPILE_FLAGS /bigobj)
    endif ()

    add_executable(${name} ${src} wamp_test.hpp ${PUBLIC_HEADERS})
    target_link_libraries(${name} PRIVATE Catch2::Catch2WithMain autobahn_cpp)
    string(REPLACE "test_" "" pure_name ${name})
    add_test(NAME ${name} COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/run_test.sh "${CMAKE_CURRENT_SOURCE_DIR}/${pure_name}" "$<TARGET_FILE:${name}>")
endfunction()

make_test(test_auth auth/auth.cpp)
make_test(test_pubsub pubsub/pubsub.cpp)
make_test(test_calls calls/calls.cpp)
