/**
 * @file threadsafe_queue.cpp
 * @author your name (you@domain.com)
 * @brief 一个通用的线程安全队列
 * @version 0.1
 * @date 2024-06-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <chrono>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <memory>
#include <thread>
#include <iostream>

template<typename T>
class threadsafe_queue {
public:
    threadsafe_queue(){}

    threadsafe_queue(const threadsafe_queue& other)
    {
        /* 如果不需要配合条件变量使用，使用unique_lock和lock_guard都可以，无差异
           无特殊需求更推荐使用lock_guard, unique_lock存在额外开销
         */
        std::lock_guard<std::mutex> lock(other.mtx);
        data_que = other.data_que;
    }

    void push(T new_value)
    {
        std::lock_guard<std::mutex> lock(mtx);
        data_que.push(new_value);
        data_cond.notify_one();
    }

    void wait_and_pop(T &value)
    {
        std::unique_lock<std::mutex> lock(mtx);
        data_cond.wait(lock, [this]{ return !data_que.empty(); });
        value = data_que.front();
        data_que.pop();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lock(mtx);
        data_cond.wait(lock, [this]{ return !data_que.empty(); });
        std::shared_ptr<T> ans(std::make_shared<T>(data_que.front()));
        data_que.pop();
        return ans;
    }

    bool try_pop(T &value)
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (data_que.empty()) {
            return false;
        }

        value = data_que.front();
        data_que.pop();
        return true;
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (data_que.empty()) {
            return std::shared_ptr<T>();
        }

        std::shared_ptr<T> ans(std::make_shared<T>(data_que.front()));
        data_que.pop();
        return ans;
    }

    bool empty() const
    {
        std::lock_guard<T> lock(mtx);
        return data_que.empty();
    }

private:
    /* 类中的mtx必须定义为mutable，因为加锁会修改互斥量，此举目的是便于在const函数中能使用该互斥量 */
    mutable std::mutex mtx;
    std::queue<T> data_que;
    std::condition_variable data_cond;
};

void test_safe_que()
{
    threadsafe_queue<int> safe_que;
    std::mutex mtx_print;

    /* 生产者 */
    std::thread producer([&]() {
        for (int i = 0;; ++i) {
            safe_que.push(i);
            {
                /* 加锁目的是，让标准输出不是乱序的 */
                std::lock_guard<std::mutex> lock_print(mtx_print);
                std::cout << "producer push data is " << i << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    });

    /* 消费者1 */
    std::thread consumer1([&]() {
        for (;;) {
            auto data = safe_que.wait_and_pop();
            {
                std::lock_guard<std::mutex> lock_print(mtx_print);
                std::cout << "consumer1 wait and pop data is " << *data << std::endl;
            } /* 在右括号，作用域结束，执行析构函数，lock_print会释放锁 */
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });

    /* 消费者2 */
    std::thread consumer2([&]() {
        for (;;) {
            auto data = safe_que.try_pop();
            if (data != nullptr) {
                {
                    std::lock_guard<std::mutex> lock_print(mtx_print);
                    std::cout << "consumer2 wait and pop data is " << *data << std::endl;
                } /* 在右括号，作用域结束，执行析构函数，lock_print会释放锁 */
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });

    producer.join();
    consumer1.join();
    consumer2.join();
}

int main()
{
    test_safe_que();
    return 0;
}
