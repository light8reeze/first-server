cmake_minimum_required(VERSION 3.20)
project(sample-server
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_STANDARD 20)

add_executable(sample-server "SampleServer.cc")

find_library(LIBURING_LIB uring REQUIRED)
find_path(LIBURING_INCLUDE liburing.h REQUIRED)
find_package(Boost 1.83 REQUIRED)

target_include_directories(sample-server PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(sample-server PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(sample-server PRIVATE ${LIBURING_INCLUDE})

target_link_libraries(sample-server PRIVATE
                                    ${LIBURING_LIB}
                                    ${Boost_LIBRARIES}
                                    braid-net
                                    braid-util
                                    braid-service
                                    braid-task)
