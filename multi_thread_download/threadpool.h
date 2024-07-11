/**
 * @file threadpool.h
 * @author lizhenlin
 * @brief 线程池
 * @version 0.1
 * @date 2024-06-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

class ThreadPool {
public:
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool& operator=(const ThreadPool &) = delete;

    static ThreadPool& instance()
    {
        static ThreadPool ins;
        return ins;
    }

    using Task = std::packaged_task<void()>;

    ~ThreadPool()
    {
        stop();
    }

    template<typename Callback, typename... Args>
    auto commit(Callback &&f, Args&&... args) -> std::future<decltype(f(args...))>
    {
        using ret_type = decltype(f(args...));
        if (is_stop.load()) 
        {
            return std::future<ret_type>{};
        }

        auto task = std::make_shared<std::packaged_task<ret_type()>>(
            std::bind(std::forward<Callback>(f), std::forward<Args>(args)...));
        
        std::future<ret_type> ret = task->get_future();
        {
            std::lock_guard<std::mutex> lock_mtx(mtx);
            tasks.emplace([task] { (*task)(); });
        }
        cv_task.notify_one();
        return ret;
    }

    int idle_thread_count()
    {
        return thread_num;
    }

private:
    ThreadPool(unsigned int num = std::thread::hardware_concurrency() + 2) : is_stop(false)
    {
        {
            thread_num = num;
        }
        start();
    }

    void start()
    {
        for (int i = 0; i < thread_num; ++i) {
            pool.emplace_back([this]() {
                while (!this->is_stop.load()) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock_mtx(mtx);
                        this->cv_task.wait(lock_mtx,
                            [this] { return this->is_stop.load() || !this->tasks.empty(); });
                        
                        if (this->tasks.empty()) {
                            return;
                        }

                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    this->thread_num--;
                    task();
                    this->thread_num++;
                }
            });
        }
    }

    void stop()
    {
        is_stop.store(true);
        cv_task.notify_all();
        for (auto &t : pool) {
            if (t.joinable()) {
                // std::cout << "join thread " << t.get_id() << std::endl;
                t.join();
            }
        }
    }

private:
    std::mutex mtx;
    std::condition_variable cv_task;
    std::atomic_bool is_stop;
    std::atomic_int thread_num;
    std::queue<Task> tasks;
    std::vector<std::thread> pool;
};

#endif
