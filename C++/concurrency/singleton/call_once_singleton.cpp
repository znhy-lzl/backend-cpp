#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

/* 为了让单例更加通用，写成模板类 */
template<typename T>
class Singleton {
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>& st) = delete;

public:
    static std::shared_ptr<T> instance()
    {
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]() {
            single = std::shared_ptr<T>(new T);
        });
        return single;
    }

    void PrinrAddress()
    {
        std::cout << single.get() << std::endl;
    }

    ~Singleton()
    {
        std::cout << "this is singleton destruct" << std::endl;
    }
protected:
    static std::shared_ptr<T> single;
};

template<typename T>
std::shared_ptr<T> Singleton<T>::single = nullptr;

/* 使用单例类 */
class LogicSystem :public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem(){}
private:
    LogicSystem(){}
};

int main()
{
    auto sp1 = LogicSystem::instance();
    auto sp2 = LogicSystem::instance();

    std::cout << "sp1 is " << sp1 << std::endl;
    std::cout << "sp2 is " << sp2 << std::endl;    
    return 0;
}
