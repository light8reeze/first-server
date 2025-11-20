add_library(first-server-thread STATIC 
							"src/AcceptorThread.cc"
							"src/IOThreadPool.cc"
							"src/IOURingThread.cc"
							"src/Thread.cc"
							"src/WorkerThread.cc")


find_package(Boost 1.83 REQUIRED COMPONENTS atomic)

target_include_directories(first-server-thread PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_include_directories(first-server-thread PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(first-server-thread PUBLIC first-server-net first-server-util)
target_link_libraries(first-server-thread PUBLIC Boost::boost Boost::atomic)