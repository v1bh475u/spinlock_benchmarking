#pragma once
#include <atomic>
#include <memory>
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__)
#include <immintrin.h>
#endif

namespace concurrency
{
    class base_lock
    {
    private:
        std::atomic<bool> locked_{false};

    public:
        void lock()
        {
            bool expected = false;
            while (!locked_.compare_exchange_strong(expected, true))
            {
                expected = false;
            }
        }
        void unlock()
        {
            locked_.store(false);
        }
    };

    class ticket_lock
    {
    private:
        std::atomic<size_t> front_{0u};
        std::atomic<size_t> back_{0u};

    public:
        void lock()
        {
            size_t ticket = back_.fetch_add(1);
            while (true)
            {
                size_t current = front_.load();
                if (current == ticket)
                    return;
            }
        }
        void unlock()
        {
            front_.fetch_add(1);
        }
    };

    class double_check_lock
    {
    private:
        std::atomic<bool> lock_{false};

    public:
        void lock()
        {
            while (true)
            {
                while (lock_.load())
                    ;
                bool expected = false;
                if (lock_.compare_exchange_strong(expected, true))
                    return;
            }
        }
        void unlock()
        {
            lock_.store(false);
        }
    };
    static inline void cpu_relax() noexcept
    {
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__)
        _mm_pause();
#elif defined(__aarch64__) || defined(__arm__)
        asm volatile("yield");
#else
        std::this_thread::yield();
#endif
    }

    class double_check_lock2
    {
    private:
        std::atomic<bool> lock_{false};

    public:
        void lock()
        {
            uint32_t spins = 1;
            while (true)
            {
                while (lock_.load())
                {
                    cpu_relax();
                }
                bool expected = false;
                if (lock_.compare_exchange_weak(expected, true))
                {
                    return;
                }

                for (uint32_t i = 0; i < spins; ++i)
                    cpu_relax();
                if (spins < (1u << 15))
                    spins <<= 1;
            }
        }

        void unlock()
        {
            lock_.store(false);
        }
    };
}
