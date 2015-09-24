include_directories(${CMAKE_SOURCE_DIR} ${Boost_INCLUDE_DIRS} ${Libmsgpack_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

set(PARAMETERS_SOURCES parameters.cpp)
set(PARAMETERS_HEADERS parameters.hpp)
add_library(examples_parameters ${PARAMETERS_SOURCES} ${PARAMETERS_HEADERS})

#set(CALLER_SOURCES caller.cpp)
set(CALLEE_SOURCES callee.cpp)
#set(PUBLISHER_SOURCES publisher.cpp)
#set(SUBSCRIBER_SOURCES subscriber.cpp)

#add_executable(caller ${CALLER_SOURCES} ${PUBLIC_HEADERS})
add_executable(callee ${CALLEE_SOURCES} ${PUBLIC_HEADERS})
#add_executable(publisher ${PUBLISHER_SOURCES} ${PUBLIC_HEADERS})
#add_executable(subscriber ${SUBSCRIBER_SOURCES} ${PUBLIC_HEADERS})

#target_link_libraries(caller examples_parameters)
target_link_libraries(callee examples_parameters)
#target_link_libraries(publisher examples_parameters)
#target_link_libraries(subscriber examples_parameters)
