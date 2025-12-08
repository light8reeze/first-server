add_library(first-server-service STATIC 
							"src/thread/IOURingThread.cc"
							"src/thread/Thread.cc"
							"src/thread/WorkerThread.cc"
							"src/service/Service.cc"
							"src/service/ServiceObject.cc"
							"src/task/TaskSerializer.cc")


find_package(Boost 1.83 REQUIRED COMPONENTS atomic)

target_include_directories(first-server-service PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_include_directories(first-server-service PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(first-server-service PUBLIC first-server-net first-server-util)
target_link_libraries(first-server-service PUBLIC Boost::boost Boost::atomic)