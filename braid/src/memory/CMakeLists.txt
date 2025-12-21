add_library(braid-memory INTERFACE)

find_package(Boost 1.83 REQUIRED)

target_include_directories(braid-memory INTERFACE
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_include_directories(braid-memory INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(braid-memory INTERFACE braid-util)
target_link_libraries(braid-memory INTERFACE braid-service)
target_link_libraries(braid-memory INTERFACE Boost::boost)
