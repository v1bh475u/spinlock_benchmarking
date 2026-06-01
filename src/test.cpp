#include "spinlock.hpp"
#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

// Test function template for any lock type
template <typename LockType> bool test_lock() {
  LockType lock;
  std::atomic<int> counter(0);

  // Test with multiple threads
  const int num_threads = 4;
  const int iterations_per_thread = 10000;

  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  // Launch threads
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back([&lock, &counter, iterations_per_thread]() {
      for (int j = 0; j < iterations_per_thread; ++j) {
        lock.lock();
        counter.fetch_add(1, std::memory_order_relaxed);
        lock.unlock();
      }
    });
  }

  // Join threads
  for (auto &thread : threads) {
    thread.join();
  }

  // Check if counter is correct
  if (counter == num_threads * iterations_per_thread) {
    std::cout << "PASSED: Counter value is " << counter << std::endl;
    return true;
  } else {
    std::cerr << "FAILED: Counter value is " << counter << " (expected "
              << num_threads * iterations_per_thread << ")" << std::endl;
    return false;
  }
}

int main() {
  bool ok = true;

  std::cout << "Testing base_lock..." << std::endl;
  ok &= test_lock<concurrency::base_lock>();

  std::cout << "Testing ticket_lock..." << std::endl;
  ok &= test_lock<concurrency::ticket_lock>();

  std::cout << "Testing double_check_lock..." << std::endl;
  ok &= test_lock<concurrency::double_check_lock>();

  std::cout << "Testing double_check_lock2..." << std::endl;
  ok &= test_lock<concurrency::double_check_lock2>();

  return ok ? 0 : 1;
}
