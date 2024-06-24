#include <iostream>
#include <memory>
#include <mutex>

class Singleton {
private:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

public:
    ~Singleton()
    {
        std::cout << "single auto delete success" << std::endl;
    }
    
    static std::shared_ptr<Singleton> instance()
    {
        if (single != nullptr) {
            return single;
        }

        mtx.lock();
        if (single != nullptr) {
            mtx.unlock();
            return single;
        }

        single = std::shared_ptr<Singleton>(new Singleton);
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
    //此时存在隐患，可以手动删除裸指针，造成崩溃
    // delete sp1.get();
}


int main()
{
    test_singleauto();
    return 0;
}
