#include "spinlock.hpp"
#include <atomic>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

// Add a new lock here to include it in every scenario, result table, and plot.
#define LOCK_BENCHMARKS(X)                                                     \
  X(concurrency::base_lock, "BaseLock")                                        \
  X(concurrency::ticket_lock, "TicketLock")                                    \
  X(concurrency::double_check_lock, "DoubleCheckLock")                         \
  X(concurrency::double_check_lock2, "DoubleCheckLock2")                       \
  X(std::mutex, "StdMutex")

#define THREAD_ARGS ->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(16)->Arg(32)

std::atomic<int> g_counter(0);
constexpr int kIterationsPerThread = 1000;

static void record_items_processed(benchmark::State &state, int num_threads) {
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) *
                          num_threads * kIterationsPerThread);
}

// High contention: every counter update enters the critical section.
template <typename LockType>
static void BM_Lock_DirectUpdate(benchmark::State &state) {
  LockType lock;
  const int num_threads = state.range(0);

  for (auto _ : state) {
    g_counter = 0;
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    for (int i = 0; i < num_threads; ++i) {
      threads.emplace_back([&lock]() {
        for (int j = 0; j < kIterationsPerThread; ++j) {
          lock.lock();
          g_counter.fetch_add(1, std::memory_order_relaxed);
          lock.unlock();
        }
      });
    }

    for (auto &thread : threads) {
      thread.join();
    }
  }

  benchmark::DoNotOptimize(g_counter);
  record_items_processed(state, num_threads);
}

// Low contention: each thread accumulates locally, then locks once.
template <typename LockType>
static void BM_Lock_AccumulatedUpdate(benchmark::State &state) {
  LockType lock;
  const int num_threads = state.range(0);

  for (auto _ : state) {
    g_counter = 0;
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    for (int i = 0; i < num_threads; ++i) {
      threads.emplace_back([&lock]() {
        int local_counter = 0;
        for (int j = 0; j < kIterationsPerThread; ++j) {
          local_counter++;
        }

        lock.lock();
        g_counter.fetch_add(local_counter, std::memory_order_relaxed);
        lock.unlock();
      });
    }

    for (auto &thread : threads) {
      thread.join();
    }
  }

  benchmark::DoNotOptimize(g_counter);
  record_items_processed(state, num_threads);
}

// clang-format off
#define REGISTER_LOCK_BENCHMARKS(LockType, LockName)                                             \
  BENCHMARK_TEMPLATE(BM_Lock_DirectUpdate, LockType)                                              \
      THREAD_ARGS->Name("DirectUpdate_" LockName);                                                \
  BENCHMARK_TEMPLATE(BM_Lock_AccumulatedUpdate, LockType)                                         \
      THREAD_ARGS->Name("AccumulatedUpdate_" LockName);
// clang-format on

LOCK_BENCHMARKS(REGISTER_LOCK_BENCHMARKS)

BENCHMARK_MAIN();
