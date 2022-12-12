#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <vector>
#include <deque>
#include <algorithm>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <iostream>

class ThreadPool {

    public:
        ThreadPool (const uint32_t num_threads) {
            n_threads = std::min(num_threads, std::thread::hardware_concurrency());
            if (n_threads < num_threads)
                std::cerr << "ThreadPool: Limiting number of threads to number of hardware threads (" << n_threads << ")" << std::endl;
            thread_pool.resize(n_threads);
            terminate_threads = false;
        }

        void add_job(std::function<void()>& fun);

        void stop();

        void start();

        bool busy();

        int num_jobs();

        uint32_t num_threads() const { return n_threads; };
    
    private:

        void thread_loop();

    private:
        bool terminate_threads;
        uint32_t n_threads;
        std::vector<std::thread> thread_pool;
        std::deque<std::function<void()>> job_pool;
        std::mutex mutex;
        std::condition_variable mutex_condition;
};

#endif