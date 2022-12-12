#include <iostream>
#include <chrono>
#include "threading/threadpool.h"

void slow_fct(){
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}


int main(){
    uint32_t nruns = 5;

    std::chrono::steady_clock::time_point begin1 = std::chrono::steady_clock::now();
    for (int i = 0; i < nruns; ++i)
        slow_fct();
    std::chrono::steady_clock::time_point end1 = std::chrono::steady_clock::now();
    std::cout << "Processing time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end1 - begin1).count() << "[ms]" << std::endl;

    ThreadPool threadpool(10);
    threadpool.start();
    std::chrono::steady_clock::time_point begin2 = std::chrono::steady_clock::now();
    for (int i = 0; i < nruns; ++i){
        std::function<void()> f = slow_fct;
        threadpool.add_job(f);
    }

    while (threadpool.busy()){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    threadpool.stop();

    std::chrono::steady_clock::time_point end2 = std::chrono::steady_clock::now();
    std::cout << "Processing time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end2 - begin2).count() << "[ms]" << std::endl;
}