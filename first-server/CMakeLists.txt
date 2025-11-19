file(GLOB_RECURSE FIRST_SERVER_SOURCES "**/**/*.cc" "*.cc")
add_executable (first-server ${FIRST_SERVER_SOURCES})

find_library(LIBURING_LIB uring REQUIRED)
find_path(LIBURING_INCLUDE liburing.h REQUIRED)

find_package(Boost 1.70 REQUIRED)

target_include_directories(first-server PRIVATE 
    ${CMAKE_SOURCE_DIR}
    ${LIBURING_INCLUDE_DIRS}
)

message(STATUS "SOURCES: ${FIRST_SERVER_SOURCES}")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET first-server PROPERTY CXX_STANDARD 20)
endif()

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

target_include_directories(first-server PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(first-server PRIVATE ${LIBURING_INCLUDE})

target_link_libraries(first-server ${LIBURING_LIB})
target_link_libraries(first-server ${Boost_LIBRARIES})