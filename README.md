# Braid

Braid는 io_uring 기반의 서비스 루프를 구성하고 작업 직렬화를 지원하는 워커 스레드를 플러그인 형태로 붙일 수 있는 C++20 네트워킹 프레임워크입니다.

## Feature
- **io_uring 기반 이벤트 루프:** `IORequestQueue`를 통해 IO 요청을 큐잉하며, `liburing`과 잠금 없는 큐로 제출 및 완료 처리를 수행합니다.
- **병렬 처리와 순서 제어:** `TaskSerializer`와 `TaskDistributor`를 통해 세션 단위 직렬화를 유지하면서도 워커 간 병렬 처리를 활용합니다.
- **워커 스레드 모델:** `WorkerThread`는 io_uring 스레드 베이스를 상속해 각 반복마다 완료 처리와 직렬화된 작업을 수행합니다.
- **구성 가능한 서비스:** 유창한 `ServiceBuilder`로 바인드 주소, 포트, 스레드 수, 세션 풀 크기, 큐 깊이를 설정한 뒤 서비스 인스턴스를 생성할 수 있습니다.

## Project Structure
- `braid/` — 핵심 서버 타깃 및 소스 트리로, 서비스/네트워크/스레드/태스크/유틸 계층을 포함합니다.
  - `net/` — io_uring 기반의 IO 요청·완료 처리 로직과 관련 헤더·구현.
  - `service/` — 서비스 객체 수명과 워커 스레드를 관리하는 구성 요소.
  - `task/` — 직렬화·분배 유틸리티로 태스크 실행 순서를 제어하는 도구.
  - `util/` — 전처리 설정과 공용 헬퍼를 모아둔 유틸리티.
- 최상위 `CMakeLists.txt` — 서버와 클라이언트 타깃을 함께 구성합니다.

## Dependency
- CMake 3.20+ 및 C++20을 지원하는 컴파일러
- `liburing` 개발 헤더와 라이브러리(필수)
- Boost 1.83 이상(헤더와 라이브러리)

## Build
1. 프로젝트 구성
   ```bash
   cmake -S . -B build
   ```
2. 서버와 클라이언트 바이너리 빌드
   ```bash
   cmake --build build
   ```

## Development
- 서비스는 현재 고정 개수의 `ServiceObject` 세션을 초기화하고 IO 요청을 워커 스레드 간 라운드로빈으로 분배합니다.
- 큐 깊이와 세션 개수는 `ServiceBuilder`를 통해 구성할 수 있어 처리량과 동시성 설정을 빠르게 실험할 수 있습니다.

## TODO
- [ ] 메모리 관리 모듈 추가 예정
