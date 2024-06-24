#include <iostream>
#include <thread>

class Singleton {
public:
    static Singleton* instance()
    {
        if (single == nullptr) {
            single = new Singleton();
        }
        return single;
    }

private:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

private:
    static Singleton* single;
};

// 类外初始化
Singleton* Singleton::single = Singleton::instance();

void func(int i)
{
    std::cout << "this is thread " << i << std::endl;
    std::cout << "instance is " << Singleton::instance() << std::endl;
}

int main()
{
    std::cout << "s1 addr is " << Singleton::instance() << std::endl;
    std::cout << "s2 addr is " << Singleton::instance() << std::endl;

    for (int i = 0; i < 3; ++i) {
        std::thread t(func, i);
        t.join();
    }
    return 0;
}
