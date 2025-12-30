#pragma once

#include <braid/task/TaskSerializer.h>
#include <braid/util/Macro.h>

#include <atomic>
#include <chrono>
#include <format>
#include <fstream>
#include <mutex>
#include <source_location>
#include <string>

namespace braid {

// 로그 레벨 열거형
enum class LogLevel : int {
    INFO = 0,
    ERROR = 1,
    FATAL = 2
};

// LogLevel을 문자열로 변환
const char* to_string(LogLevel level);

// LogHandler: Singleton + TaskSerializer 기반 로그 처리기
class LogHandler final : public TaskSerializer {
    NON_COPYABLE(LogHandler)

private:
    // Singleton
    static LogHandler* instance_;
    static std::once_flag init_flag_;

    LogHandler();
    ~LogHandler() override;

public:
    // Singleton 인스턴스 접근
    static LogHandler& get_instance();

    // 초기화 (파일 경로, 최소 로그 레벨)
    void initialize(const std::string& file_path = "", LogLevel min_level = LogLevel::INFO);

    // 런타임 설정
    void set_min_level(LogLevel level);
    LogLevel get_min_level() const;

    void enable_file_output(const std::string& file_path);
    void disable_file_output();
    bool is_file_enabled() const;

    // TaskSerializer를 통해 호출될 write_log (public으로 선언)
    void write_log(LogLevel level, std::string timestamp,
                   std::string location, std::string message);

private:
    // 헬퍼 메서드
    std::string format_log(LogLevel level, const std::string& timestamp,
                          const std::string& location, const std::string& message);
    void write_to_stdout(const std::string& formatted);
    void write_to_file(const std::string& formatted);

private:
    // 설정
    std::atomic<LogLevel> min_level_;
    std::atomic<bool> file_enabled_;

    // 파일 출력 (mutex 불필요 - TaskSerializer가 직렬화 보장)
    std::ofstream file_stream_;
    std::string file_path_;
};

// 타임스탬프 캡처 헬퍼 함수
inline std::string capture_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now;
    localtime_r(&time_t_now, &tm_now);

    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
                  tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday,
                  tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);

    return std::string(buffer);
}


constexpr std::string_view get_filename(std::string_view path) {
    const size_t pos = path.find_last_of("/\\");
    return (pos == std::string_view::npos)
        ? path
        : path.substr(pos + 1);
}


// 내부 구현 함수 (source_location을 받음) - 포맷 문자열 + 가변 인자
template<typename... Args>
void LogImpl(LogLevel level, const std::source_location& loc,
             std::format_string<Args...> fmt, Args&&... args) {
    LogHandler& handler = LogHandler::get_instance();

    // 조기 필터링
    if (level < handler.get_min_level()) {
        return;
    }

    // 타임스탬프 캡처
    std::string timestamp = capture_timestamp();

    // 소스 위치 포맷팅
    std::string location = std::format("[{}:{} {}]",
        get_filename(loc.file_name()), loc.line(), loc.function_name());

    // 메시지 포맷팅 (std::format 사용)
    std::string message = std::format(fmt, std::forward<Args>(args)...);

    // 비동기 로그 요청
    handler.request_task(&handler, &LogHandler::write_log,
                        level, std::move(timestamp),
                        std::move(location), std::move(message));
}

// 메시지만 있는 경우 (포맷 인자 없음)
inline void LogImpl(LogLevel level, const std::source_location& loc,
                    const char* message) {
    LogHandler& handler = LogHandler::get_instance();

    // 조기 필터링
    if (level < handler.get_min_level()) {
        return;
    }

    // 타임스탬프 캡처
    std::string timestamp = capture_timestamp();

    // 소스 위치 포맷팅
    std::string location = std::format("[{}:{} {}]",
        get_filename(loc.file_name()), loc.line(), loc.function_name());

    // 비동기 로그 요청
    handler.request_task(&handler, &LogHandler::write_log,
                        level, std::move(timestamp),
                        std::move(location), std::string(message));
}


}  // namespace braid

// 사용자 인터페이스 매크로
#define LOG(level, ...) \
    ::braid::LogImpl(level, std::source_location::current(), __VA_ARGS__)

// 편의 매크로
#define LOG_INFO(...)  LOG(::braid::LogLevel::INFO, __VA_ARGS__)
#define LOG_ERROR(...) LOG(::braid::LogLevel::ERROR, __VA_ARGS__)
#define LOG_FATAL(...) LOG(::braid::LogLevel::FATAL, __VA_ARGS__)
