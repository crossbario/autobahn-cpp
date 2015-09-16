include_directories(${CMAKE_SOURCE_DIR} ${Boost_INCLUDE_DIRS} ${Libmsgpack_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

set(CALLER_SOURCES caller.cpp)
set(CALLEE_SOURCES callee.cpp)
set(PUBLISHER_SOURCES publisher.cpp)
set(SUBSCRIBER_SOURCES subscriber.cpp)

add_executable(component_caller ${CALLER_SOURCES})
add_executable(component_callee ${CALLEE_SOURCES})
add_executable(component_publisher ${PUBLISHER_SOURCES})
add_executable(component_subscriber ${SUBSCRIBER_SOURCES})

target_link_libraries(component_caller example_utilities)
target_link_libraries(component_callee example_utilities)
target_link_libraries(component_publisher example_utilities)
target_link_libraries(component_subscriber example_utilities)
