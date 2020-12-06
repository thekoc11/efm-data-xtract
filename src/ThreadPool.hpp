//
// Created by theko on 06/12/20.
//

#ifndef PROJ_THREADPOOL_HPP
#define PROJ_THREADPOOL_HPP

#include <condition_variable>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>

class ThreadPool{
private:
    std::vector<std::thread> threadVect;
    uint num_threads;

public:
    explicit ThreadPool(int n)  : num_threads(n){}
};

#endif //PROJ_THREADPOOL_HPP
