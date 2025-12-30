#include <braid/util/LogHandler.h>
#include <braid/task/TaskDistributor.h>

#include <iostream>

namespace braid {

// Static 멤버 초기화
LogHandler* LogHandler::instance_ = nullptr;
std::once_flag LogHandler::init_flag_;

// LogLevel을 문자열로 변환
const char* to_string(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:  return "INFO";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

// 생성자
LogHandler::LogHandler()
    : TaskSerializer()
    , min_level_(LogLevel::INFO)
    , file_enabled_(false) {
    // TaskDistributor에 등록 (첫 로그 시 자동 처리됨)
}

// 소멸자
LogHandler::~LogHandler() {
    // TaskSerializer 소멸 준비
    set_destroy();

    // 파일 스트림 닫기
    if (file_stream_.is_open()) {
        file_stream_.flush();
        file_stream_.close();
    }
}

// Singleton 인스턴스 접근
LogHandler& LogHandler::get_instance() {
    std::call_once(init_flag_, []() {
        instance_ = new LogHandler();
    });
    return *instance_;
}

// 초기화
void LogHandler::initialize(const std::string& file_path, LogLevel min_level) {
    min_level_.store(min_level, std::memory_order_relaxed);

    if (!file_path.empty()) {
        enable_file_output(file_path);
    }
}

// 최소 로그 레벨 설정
void LogHandler::set_min_level(LogLevel level) {
    min_level_.store(level, std::memory_order_relaxed);
}

// 최소 로그 레벨 조회
LogLevel LogHandler::get_min_level() const {
    return min_level_.load(std::memory_order_relaxed);
}

// 파일 출력 활성화
void LogHandler::enable_file_output(const std::string& file_path) {
    // 기존 파일 스트림 닫기
    if (file_stream_.is_open()) {
        file_stream_.close();
    }

    file_path_ = file_path;
    file_stream_.open(file_path_, std::ios::out | std::ios::app);

    if (!file_stream_.is_open()) {
        std::cerr << "[LogHandler] Failed to open log file: " << file_path_ << std::endl;
        file_enabled_.store(false, std::memory_order_release);
        return;
    }

    file_enabled_.store(true, std::memory_order_release);
}

// 파일 출력 비활성화
void LogHandler::disable_file_output() {
    file_enabled_.store(false, std::memory_order_release);

    if (file_stream_.is_open()) {
        file_stream_.flush();
        file_stream_.close();
    }
}

// 파일 출력 활성화 여부
bool LogHandler::is_file_enabled() const {
    return file_enabled_.load(std::memory_order_acquire);
}

// 로그 쓰기 (TaskSerializer를 통해 WorkerThread에서 호출됨)
void LogHandler::write_log(LogLevel level, std::string timestamp,
                           std::string location, std::string message) {
    // 포맷팅
    std::string formatted = format_log(level, timestamp, location, message);

    // stdout 출력
    write_to_stdout(formatted);

    // 파일 출력 (활성화된 경우)
    if (is_file_enabled()) {
        write_to_file(formatted);
    }
}

// 로그 포맷팅
std::string LogHandler::format_log(LogLevel level, const std::string& timestamp,
                                   const std::string& location, const std::string& message) {
    // 형식: 2025-12-30 12:34:56 [INFO] [file.cc:42 func_name] : message
    return std::format("{} [{}] {} : {}",
                      timestamp, to_string(level), location, message);
}

// stdout 출력
void LogHandler::write_to_stdout(const std::string& formatted) {
    // std::cout은 OS 레벨에서 thread-safe (PIPE_BUF 이하 크기)
    // endl을 사용하여 flush 보장
    std::cout << formatted << std::endl;
}

// 파일 출력
void LogHandler::write_to_file(const std::string& formatted) {
    // TaskSerializer가 직렬화를 보장하므로 mutex 불필요

    if (!file_stream_.is_open() || !file_stream_.good()) {
        // 파일 스트림이 유효하지 않으면 파일 출력 비활성화
        file_enabled_.store(false, std::memory_order_release);
        std::cerr << "[LogHandler] File stream is not valid, disabling file output" << std::endl;
        return;
    }

    file_stream_ << formatted << std::endl;

    // 크래시 안전성을 위해 매 로그마다 flush
    // (향후 배치 플러시로 최적화 가능)
    file_stream_.flush();

    // 쓰기 에러 체크
    if (file_stream_.fail()) {
        file_enabled_.store(false, std::memory_order_release);
        std::cerr << "[LogHandler] File write error, disabling file output" << std::endl;
    }
}

}  // namespace braid
