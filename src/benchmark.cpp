#include <benchmark/benchmark.h>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include "spinlock.hpp"

// Shared counter for contention testing
std::atomic<int> g_counter(0);

// Benchmark for direct counter updates - high contention
template <typename LockType>
static void BM_SpinLock_DirectUpdate(benchmark::State &state)
{
    LockType lock;
    g_counter = 0;

    // Create threads
    const int num_threads = state.range(0);
    const int iterations_per_thread = 1000;

    for (auto _ : state)
    {
        std::vector<std::thread> threads;
        threads.reserve(num_threads);

        // Launch threads - each iteration acquires lock
        for (int i = 0; i < num_threads; ++i)
        {
            threads.emplace_back([&lock, iterations_per_thread]()
                                 {
                for (int j = 0; j < iterations_per_thread; ++j) {
                    lock.lock();
                    g_counter.fetch_add(1, std::memory_order_relaxed);
                    lock.unlock();
                } });
        }

        // Join threads
        for (auto &thread : threads)
        {
            thread.join();
        }
    }

    // Verify correctness
    benchmark::DoNotOptimize(g_counter);
    state.SetItemsProcessed(state.iterations() * num_threads * iterations_per_thread);
}

// Benchmark for local accumulation before update - low contention
template <typename LockType>
static void BM_SpinLock_AccumulatedUpdate(benchmark::State &state)
{
    LockType lock;
    g_counter = 0;

    // Create threads
    const int num_threads = state.range(0);
    const int iterations_per_thread = 1000;

    for (auto _ : state)
    {
        std::vector<std::thread> threads;
        threads.reserve(num_threads);

        // Launch threads - accumulate locally, update once
        for (int i = 0; i < num_threads; ++i)
        {
            threads.emplace_back([&lock, iterations_per_thread]()
                                 {
                int local_counter = 0;
                // Accumulate locally without locking
                for (int j = 0; j < iterations_per_thread; ++j) {
                    local_counter++;
                }
                // Update global counter once with lock
                lock.lock();
                g_counter.fetch_add(local_counter, std::memory_order_relaxed);
                lock.unlock(); });
        }

        // Join threads
        for (auto &thread : threads)
        {
            thread.join();
        }
    }

    // Verify correctness
    benchmark::DoNotOptimize(g_counter);
    state.SetItemsProcessed(state.iterations() * num_threads * iterations_per_thread);
}

// Compare against std::mutex for reference - direct updates
static void BM_StdMutex_DirectUpdate(benchmark::State &state)
{
    std::mutex mutex;
    g_counter = 0;

    // Create threads
    const int num_threads = state.range(0);
    const int iterations_per_thread = 1000;

    for (auto _ : state)
    {
        std::vector<std::thread> threads;
        threads.reserve(num_threads);

        // Launch threads
        for (int i = 0; i < num_threads; ++i)
        {
            threads.emplace_back([&mutex, iterations_per_thread]()
                                 {
                for (int j = 0; j < iterations_per_thread; ++j) {
                    std::lock_guard<std::mutex> lock(mutex);
                    g_counter.fetch_add(1, std::memory_order_relaxed);
                } });
        }

        // Join threads
        for (auto &thread : threads)
        {
            thread.join();
        }
    }

    benchmark::DoNotOptimize(g_counter);
    state.SetItemsProcessed(state.iterations() * num_threads * iterations_per_thread);
}

// Compare against std::mutex for reference - accumulated updates
static void BM_StdMutex_AccumulatedUpdate(benchmark::State &state)
{
    std::mutex mutex;
    g_counter = 0;

    // Create threads
    const int num_threads = state.range(0);
    const int iterations_per_thread = 1000;

    for (auto _ : state)
    {
        std::vector<std::thread> threads;
        threads.reserve(num_threads);

        // Launch threads
        for (int i = 0; i < num_threads; ++i)
        {
            threads.emplace_back([&mutex, iterations_per_thread]()
                                 {
                int local_counter = 0;
                // Accumulate locally without locking
                for (int j = 0; j < iterations_per_thread; ++j) {
                    local_counter++;
                }
                // Update global counter once with lock
                std::lock_guard<std::mutex> lock(mutex);
                g_counter.fetch_add(local_counter, std::memory_order_relaxed); });
        }

        // Join threads
        for (auto &thread : threads)
        {
            thread.join();
        }
    }

    benchmark::DoNotOptimize(g_counter);
    state.SetItemsProcessed(state.iterations() * num_threads * iterations_per_thread);
}

// Define benchmarks for direct updates
BENCHMARK_TEMPLATE(BM_SpinLock_DirectUpdate, concurrency::base_lock)
    ->Arg(1)
    ->Arg(2)
    ->Arg(4)
    ->Arg(8)
    ->Arg(16)
    ->Name("DirectUpdate_BaseLock");

BENCHMARK_TEMPLATE(BM_SpinLock_DirectUpdate, concurrency::ticket_lock)
    ->Arg(1)
    ->Arg(2)
    ->Arg(4)
    ->Arg(8)
    ->Arg(16)
    ->Name("DirectUpdate_TicketLock");

BENCHMARK_TEMPLATE(BM_SpinLock_DirectUpdate, concurrency::double_check_lock)
    ->Arg(1)
    ->Arg(2)
    ->Arg(4)
    ->Arg(8)
    ->Arg(16)
    ->Name("DirectUpdate_DoubleCheckLock");

BENCHMARK(BM_StdMutex_DirectUpdate)
    ->Arg(1)
    ->Arg(2)
    ->Arg(4)
    ->Arg(8)
    ->Arg(16)
    ->Name("DirectUpdate_StdMutex");

// Define benchmarks for accumulated updates
BENCHMARK_TEMPLATE(BM_SpinLock_AccumulatedUpdate, concurrency::base_lock)
    ->Arg(1)
    ->Arg(2)
    ->Arg(4)
    ->Arg(8)
    ->Arg(16)
    ->Name("AccumulatedUpdate_BaseLock");

BENCHMARK_TEMPLATE(BM_SpinLock_AccumulatedUpdate, concurrency::ticket_lock)
    ->Arg(1)
    ->Arg(2)
    ->Arg(4)
    ->Arg(8)
    ->Arg(16)
    ->Name("AccumulatedUpdate_TicketLock");

BENCHMARK_TEMPLATE(BM_SpinLock_AccumulatedUpdate, concurrency::double_check_lock)
    ->Arg(1)
    ->Arg(2)
    ->Arg(4)
    ->Arg(8)
    ->Arg(16)
    ->Name("AccumulatedUpdate_DoubleCheckLock");

BENCHMARK(BM_StdMutex_AccumulatedUpdate)
    ->Arg(1)
    ->Arg(2)
    ->Arg(4)
    ->Arg(8)
    ->Arg(16)
    ->Name("AccumulatedUpdate_StdMutex");

BENCHMARK_MAIN();
