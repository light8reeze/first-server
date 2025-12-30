#include <braid/message/MessageDispatcher.h>
#include <braid/message/MessageHeader.h>
#include <braid/service/Service.h>
#include <braid/service/ServiceBuilder.h>
#include <braid/task/TaskSerializer.h>
#include <braid/util/LogHandler.h>
#include <algorithm>
#include <chrono>
#include <atomic>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <string_view>
#include <unordered_map>
#include <cassert>


using namespace braid;

// 1-1 Write TestMessage class derived from MessageHeader
struct TestMessage : public MessageHeader {
  char data[1024];
};

// 1-3 Write task function with timestamp parameter
// 1-4 (Async part) Task execution to echo back
void timestamp_task(Actor *actor, long long timestamp, std::string message) {
  long long seq = -1;

  // Expected format: "seq=<n>; ts=<millis>; <payload>"
  std::string_view view(message);
  if (view.rfind("seq=", 0) == 0) {
    size_t first_sep = view.find(';');
    if (first_sep != std::string_view::npos) {
      std::string seq_str(view.substr(4, first_sep - 4));
      try {
        seq = std::stoll(seq_str);
      } catch (...) {
        seq = -1;
      }
    }
  }

  static std::mutex seq_mutex;

  bool sequential_ok = false;
  long long expected_seq = -1;

  thread_local std::mt19937 rng([] {
    std::random_device rd;
    std::seed_seq seed{rd(), rd(), rd(), rd()};
    return std::mt19937(seed);
  }());
  std::uniform_int_distribution<int> dist(1, 4000);
  int total_send_size = dist(rng);

  std::string header = sequential_ok ? "[OK] " : "[ERR] ";
  header += "seq=" + std::to_string(seq) + "; ";
  header += "sequential=" + std::string(sequential_ok ? "true" : "false") + "; ";
  header += "size=" + std::to_string(total_send_size) + "; ";
  header += message;

  // Send randomized-size payload (1..4000 bytes) by splitting into multiple
  // SendBuffer chunks (SendBuffer has a fixed max capacity).
  constexpr int kChunkSize = 1024;
  int remaining = total_send_size;
  int offset = 0;

  while (remaining > 0) {
    int chunk = std::min(kChunkSize, remaining);
    ObjectPtr<SendBuffer> send_buffer = new SendBuffer();

    // Fill with deterministic bytes, then overlay the header at the start.
    std::string payload(static_cast<size_t>(chunk), 'x');
    if (offset < static_cast<int>(header.size())) {
      int copy = std::min(chunk, static_cast<int>(header.size()) - offset);
      payload.replace(0, static_cast<size_t>(copy), header.data() + offset,
                      static_cast<size_t>(copy));
    }

    send_buffer->write(payload.data(), static_cast<int>(payload.size()));
    actor->request_send(send_buffer);

    remaining -= chunk;
    offset += chunk;
  }
}

int main(int argc, char *argv[]) {
  LOG_INFO("Starting Sample Server...");

  // Initialize Dispatcher
  // 1-2 Write MessageHandler event that echo to client and add to
  // MessageDispatcher
  g_dispatcher.register_handler(
      1, [](std::span<char> &data, ObjectPtr<Actor> actor) {
        MessageHeader *hdr = parse_message<MessageHeader>(data);
        if (!hdr || hdr->size < static_cast<int>(sizeof(MessageHeader)) ||
            hdr->size > static_cast<int>(data.size())) {
          LOG_ERROR("Failed to parse message");
          return;
        }

        std::string message_content;
        if (hdr->size > static_cast<int>(sizeof(MessageHeader))) {
          message_content.assign(data.data() + sizeof(MessageHeader));
		}
// 		std::cout << "[Handler] Received: " << message_content << std::endl;

        // 1-4 request task written 1-3 in the step of 1-2
        long long current_time =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count();

        // Pass actor, timestamp, and copied message data to the task
        static std::atomic<long long> g_seq{0};
        long long seq = g_seq.fetch_add(1, std::memory_order_relaxed);
        std::string wrapped_message =
            "seq=" + std::to_string(seq) + "; ts=" + std::to_string(current_time) +
            "; " + message_content;

        timestamp_task(actor.get(), current_time, wrapped_message);

        // Mutate locals after enqueue to catch accidental reference-captures.
        current_time = -1;
        wrapped_message = "mutated";
      });

  auto service = braid::ServiceBuilder<braid::Service>::create_builder()
                     .set_address("", 4832)
                     .set_backlog(4096)
                     .set_thread_count(36)
                     .set_session_count(10000)
                	  .set_queue_depth_per_thread(10000)
                     .build();

  if (!service) {
    LOG_ERROR("Failed to build service");
    return 1;
  }

  if (!service->initialize()) {
    LOG_ERROR("Failed to initialize service");
    return 1;
  }

  service->run();

  return 0;
}
