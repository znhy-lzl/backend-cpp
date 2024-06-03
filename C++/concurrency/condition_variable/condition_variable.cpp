/**
 * @file condition_variable.cpp
 * @author lizhenlin
 * @brief 条件变量基本使用
 * @details 使用条件变量，实现交替打印数字1和2
 * @version 0.1
 * @date 2024-06-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

int num = 1;
std::mutex mtx;
std::condition_variable cv_a;
std::condition_variable cv_b;

void func()
{
    std::thread t1([]() {
        for (;;) {
            std::unique_lock<std::mutex> lock(mtx);
            
            /* 写法一 */
            // while (num != 1) {
            //     cv_a.wait(lock);
            // }

            /* 写法二 */
            cv_a.wait(lock, []() { return num == 1; });

            std::cout << "thread A prin 1......" << std::endl;
            ++num;
            cv_b.notify_one();
        }
    });

    std::thread t2([]() {
        for (;;) {
            std::unique_lock<std::mutex> lock(mtx);
            cv_b.wait(lock, []() { return num == 2; });

            std::cout << "thread B prin 2......" << std::endl;
            --num;
            cv_a.notify_one();
        }
    });

    t1.join();
    t2.join();
}

int main()
{
    func();
    return 0;
}
