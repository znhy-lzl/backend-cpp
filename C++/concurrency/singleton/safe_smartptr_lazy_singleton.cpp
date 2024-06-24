#include <iostream>
#include <memory>
#include <mutex>

class Singleton;     /* 前向声明 */

class SafeDeletor {
public:
    void operator()(Singleton *sf)
    {
        std::cout << "this is safe deletor operator()" << std::endl;
        delete sf;
    }
};

class Singleton {
    friend class SafeDeletor;    /* 定义友元类 */
private:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    ~Singleton()
    {
        std::cout << "single auto delete success" << std::endl;
    }
public:

    
    static std::shared_ptr<Singleton> instance()
    {
        /* 1处 */
        if (single != nullptr) {
            return single;
        }

        mtx.lock();
        /* 2处 */
        if (single != nullptr) {
            mtx.unlock();
            return single;
        }

        /* 3处 */
        single = std::shared_ptr<Singleton>(new Singleton, SafeDeletor());   /* 额外指定删除器 */
        mtx.unlock();

        return single;
    }
private:
    static std::shared_ptr<Singleton> single;
    static std::mutex mtx;
};

/* 测试如下 */
std::shared_ptr<Singleton> Singleton::single = nullptr;
std::mutex Singleton::mtx;

void test_singleauto()
{
    auto sp1 = Singleton::instance();
    auto sp2 = Singleton::instance();
    std::cout << "sp1 is " << sp1 << std::endl;
    std::cout << "sp2 is " << sp2 << std::endl;
}


int main()
{
    test_singleauto();
    return 0;
}
