/**
 * @file test.cpp
 * @author lizhenlin
 * @brief 测试线程池的使用
 * @version 0.1
 * @date 2024-06-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

#include "threadpool.h"


int main()
{
    auto &m1 = ThreadPool::instance();
    auto &m2 = ThreadPool::instance();
    std::cout << "m1 is " << &m1 << std::endl;
    std::cout << "m2 is " << &m2 << std::endl;

    int m = 0;
    ThreadPool::instance().commit([](int &m) {
        m = 1024;
        std::cout << "inner set m is " << m << std::endl;
    }, std::ref(m));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "m is " << m << std::endl;

    /* 错误传参, 直接传m，虽然是左值传入，但是读bind代码，会发现内部会使用decay去掉m的左值属性
     * 所以线程内修改，并不会修改主线程中的m
     */
    // ThreadPool::instance().commit([](int &m) {
    //     m = 1024;
    //     std::cout << "inner set m is " << m << std::endl;
    // }, m);
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    // std::cout << "m is " << m << std::endl;

    return 0;
}
