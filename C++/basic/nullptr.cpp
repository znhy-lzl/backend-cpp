#include <cstddef>
#include <iostream>
#include <type_traits>

void foo(char *);
void foo(int);

int main()
{
    /* NULL 不同于 0, NULL 是 long类型, 0 是 int 类型
       NULL 也不同于 nullptr
    */
    if (std::is_same_v<decltype(NULL), decltype(0)>) {   /* is_same_v = is_same::value */
        std::cout << "NULL == 0" << std::endl;
    }

    if (std::is_same_v<decltype(NULL), decltype((void*)0)>) {
        std::cout << "NULL == (void *)0" << std::endl;
    }

    if (std::is_same_v<decltype(NULL), decltype(nullptr)>) {
        std::cout << "NULL == nullptr" << std::endl;
    }

    foo(0);
    // foo(NULL);    /* 无法编译通过，存在歧义, NULL 在C++中被定义为 0 */
    foo(nullptr);
    return 0;
}

void foo(char *) {
    std::cout << "foo(char*) is called" << std::endl;
}

void foo(int) {
    std::cout << "foo(int) is called" << std::endl;
}
