add_library(first-server-net STATIC 
							"src/IOCompletion.cc"
							"src/IOOperationAccept.cc"
							"src/IOOperationNop.cc"
							"src/IOOperationRecv.cc"
							"src/IOOperationSend.cc"
							"src/IORequestQueue.cc")

find_library(LIBURING_LIB uring REQUIRED)
find_path(LIBURING_INCLUDE liburing.h REQUIRED)
find_package(Boost 1.83 REQUIRED COMPONENTS atomic)

target_include_directories(first-server-net PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_include_directories(first-server-net PRIVATE ${LIBURING_INCLUDE})
target_include_directories(first-server-net PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(first-server-net PUBLIC first-server-util)
target_link_libraries(first-server-net PUBLIC Boost::boost Boost::atomic)