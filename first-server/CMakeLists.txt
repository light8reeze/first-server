# CMakeList.txt: FirstServer에 대한 CMake 프로젝트, 여기에 소스를 포함하고
# 프로젝트 특정 논리를 정의합니다.
#
# 이 프로젝트의 실행 파일에 소스를 추가합니다.
file(GLOB_RECURSE FIRST_SERVER_SOURCES "**/**/*.cc" "*.cc")
add_executable (first-server ${FIRST_SERVER_SOURCES})

message(STATUS "SOURCES: ${FIRST_SERVER_SOURCES}")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET first-server PROPERTY CXX_STANDARD 20)
endif()

find_library(LIBURING_LIB uring REQUIRED)
find_path(LIBURING_INCLUDE liburing.h REQUIRED)

target_link_libraries(first-server ${LIBURING_LIB})
target_include_directories(first-server PRIVATE ${LIBURING_INCLUDE})