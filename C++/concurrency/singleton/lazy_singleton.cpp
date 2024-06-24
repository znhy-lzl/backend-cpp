#include <iostream>
#include <mutex>
#include <thread>

class Singleton {
private:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

public:
    static Singleton *instance()
    {
        if (single != nullptr) {
            return single;
        }

        mtx.lock();
        if (single != nullptr) {
            mtx.unlock();
            return single;
        }

        single = new Singleton();
        mtx.unlock();

        return single;
    }
private:
    static Singleton *single;
    static std::mutex mtx;
};

/* 测试如下 */
Singleton* Singleton::single = nullptr;
std::mutex Singleton::mtx;

void thread_func_lazy(int i)
{
    std::cout << "this is lazy thread " << i << std::endl;
    std::cout << "instance is " << Singleton::instance() << std::endl;
}


int main()
{
    for (int i = 0; i < 3; ++i) {
        std::thread t(thread_func_lazy, i);
        t.join();
    }
    return 0;
}
