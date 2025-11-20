cmake_minimum_required(VERSION 3.20)
project(first-server
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_STANDARD 20)

add_executable(first-server "FirstServer.cc")
add_subdirectory("src")

find_library(LIBURING_LIB uring REQUIRED)
find_path(LIBURING_INCLUDE liburing.h REQUIRED)
find_package(Boost 1.83 REQUIRED)

target_include_directories(first-server PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(first-server PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(first-server PRIVATE ${LIBURING_INCLUDE})

target_link_libraries(first-server PRIVATE
                                    ${LIBURING_LIB}
                                    ${Boost_LIBRARIES}
                                    first-server-net
                                    first-server-thread)
