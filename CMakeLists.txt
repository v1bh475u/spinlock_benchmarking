cmake_minimum_required(VERSION 3.14)
project(spinlock_benchmark CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
find_package(Threads REQUIRED)

include(FetchContent)
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.8.2
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(benchmark)


include_directories(include)

add_executable(spinlock_bench src/benchmark.cpp)
target_link_libraries(spinlock_bench PRIVATE benchmark::benchmark Threads::Threads)

add_executable(spinlock_test src/test.cpp)
target_link_libraries(spinlock_test PRIVATE Threads::Threads)

add_test(NAME spinlock_correctness COMMAND spinlock_test)
