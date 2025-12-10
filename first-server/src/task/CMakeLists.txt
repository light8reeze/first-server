add_library(first-server-task STATIC 
							"src/task/TaskSerializer.cc"
							"src/task/TaskDistributor.cc")


find_package(Boost 1.83 REQUIRED COMPONENTS atomic)

target_include_directories(first-server-task PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_include_directories(first-server-task PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(first-server-task PUBLIC Boost::boost Boost::atomic)