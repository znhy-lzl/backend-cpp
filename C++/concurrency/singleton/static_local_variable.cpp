
#include <iostream>

class Singleton {
public:
    static Singleton& instance()
    {
        static Singleton singleton;
        return singleton;
    }

private:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};

int main()
{
    std::cout << "s1 addr is " << &Singleton::instance() << std::endl;
    std::cout << "s2 addr is " << &Singleton::instance() << std::endl;
    return 0;
}
