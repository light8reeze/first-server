add_library(braid-service STATIC 
							"src/thread/IOURingThread.cc"
							"src/thread/Thread.cc"
							"src/thread/WorkerThread.cc"
							"src/service/Service.cc"
							"src/service/ServiceSession.cc"
							"src/service/Actor.cc"
							"src/message/MessageDispatcher.cc"
							"src/net/IOOperationSendBuffer.cc"
							)


find_package(Boost 1.83 REQUIRED COMPONENTS atomic)

target_include_directories(braid-service PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_include_directories(braid-service PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(braid-service PUBLIC braid-net braid-util)
target_link_libraries(braid-service PUBLIC Boost::boost Boost::atomic)
target_link_libraries(braid-service PUBLIC braid-task)