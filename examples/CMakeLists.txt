include_directories(${CMAKE_SOURCE_DIR} ${Boost_INCLUDE_DIRS}})
link_libraries(${Boost_LIBRARIES})

set(SOURCES parameters.cpp)
set(HEADERS parameters.hpp)
add_library(example_utilities ${SOURCES} ${HEADERS})

add_subdirectory(components)
add_subdirectory(sessions)
