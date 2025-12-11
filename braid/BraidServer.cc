#include <service/Service.h>
#include <memory>

int main() {

	std::shared_ptr<first::Service> game_service = first::ServiceBuilder<first::Service>::create_builder()
												.set_address("", 4832)
												.set_thread_count(16)
												.set_session_count(100)
												.build();

	game_service->initialize();

	game_service->run();

	return 0;
}