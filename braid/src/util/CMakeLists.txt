add_library(first-server-util INTERFACE)

target_include_directories(first-server-util INTERFACE
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)
