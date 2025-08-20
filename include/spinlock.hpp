#pragma once
#include <atomic>
#include <memory>

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
            while (!locked_.compare_exchange_weak(expected, true))
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
                if (lock_.compare_exchange_weak(expected, true))
                    return;
            }
        }
        void unlock()
        {
            lock_.store(false);
        }
    };
}
