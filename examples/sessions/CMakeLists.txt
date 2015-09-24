include_directories(${CMAKE_SOURCE_DIR} ${Boost_INCLUDE_DIRS} ${Libmsgpack_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

set(CALLER_SOURCES caller.cpp)
set(CALLEE_SOURCES callee.cpp)
set(PUBLISHER_SOURCES publisher.cpp)
set(SUBSCRIBER_SOURCES subscriber.cpp)

add_executable(session_caller ${CALLER_SOURCES})
add_executable(session_callee ${CALLEE_SOURCES})
add_executable(session_publisher ${PUBLISHER_SOURCES})
add_executable(session_subscriber ${SUBSCRIBER_SOURCES})

target_link_libraries(session_caller example_utilities)
target_link_libraries(session_callee example_utilities)
target_link_libraries(session_publisher example_utilities)
target_link_libraries(session_subscriber example_utilities)
