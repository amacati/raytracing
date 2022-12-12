#include "threadpool.h"
#include <mutex>
#include <thread>
#include <functional>


void ThreadPool::add_job(std::function<void()>& fun){
    {
        std::lock_guard<std::mutex> lock(mutex);
        job_pool.push_back(fun);
    }
    mutex_condition.notify_one();
}

void ThreadPool::start(){
    for (uint32_t i = 0; i < n_threads; ++i){
        thread_pool.push_back(std::thread(&ThreadPool::thread_loop, this));
    }
}

void ThreadPool::stop(){
    {
        std::unique_lock<std::mutex> lock(mutex);
        terminate_threads = true;
    }
    mutex_condition.notify_all();
    for (std::thread& active_thread : thread_pool) {
        if (active_thread.joinable())
            active_thread.join();
    }
    thread_pool.clear();
}

int ThreadPool::num_jobs(){
    int n_jobs;
    {
        std::lock_guard<std::mutex> lock(mutex);
        n_jobs = job_pool.size();
    }
    return n_jobs;
}

bool ThreadPool::busy() {
    bool is_busy;
    {
        std::lock_guard<std::mutex> lock(mutex);
        is_busy = !job_pool.empty();
    }
    return is_busy;
}

void ThreadPool::thread_loop(){
    while(true){
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(mutex);
            mutex_condition.wait(lock, [this]{ return !job_pool.empty() || terminate_threads;});
            if (terminate_threads)
                return;
            job = job_pool.front();
            job_pool.pop_front();
        }
        job();
    }
}