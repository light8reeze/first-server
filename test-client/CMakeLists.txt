
add_executable (test-client "TestClient.cc")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET test-client PROPERTY CXX_STANDARD 20)
endif()