#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <cassert>
#include <mutex>
#include "spinlock.hpp"

// Test function template for any lock type
template <typename LockType>
void test_lock()
{
    LockType lock;
    std::atomic<int> counter(0);

    // Test with multiple threads
    const int num_threads = 4;
    const int iterations_per_thread = 10000;

    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    // Launch threads
    for (int i = 0; i < num_threads; ++i)
    {
        threads.emplace_back([&lock, &counter, iterations_per_thread]()
                             {
            for (int j = 0; j < iterations_per_thread; ++j) {
                lock.lock();
                counter.fetch_add(1, std::memory_order_relaxed);
                lock.unlock();
            } });
    }

    // Join threads
    for (auto &thread : threads)
    {
        thread.join();
    }

    // Check if counter is correct
    if (counter == num_threads * iterations_per_thread)
    {
        std::cout << "PASSED: Counter value is " << counter << std::endl;
    }
    else
    {
        std::cerr << "FAILED: Counter value is " << counter
                  << " (expected " << num_threads * iterations_per_thread << ")" << std::endl;
    }
}

int main()
{
    std::cout << "Testing base_lock..." << std::endl;
    test_lock<concurrency::base_lock>();

    std::cout << "Testing ticket_lock..." << std::endl;
    test_lock<concurrency::ticket_lock>();

    return 0;
}
