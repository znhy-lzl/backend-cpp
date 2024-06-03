/* 
    C++11规定，允许将变量声明为constexpr 类型以便由编译器来验证变量是否是一个常量表达式
    常量表达式：值不会改变且在编译阶段就能得到计算结果的表达式
*/
#include <iostream>

#define LEN 10

int len_foo()
{
    int i = 2;
    return i;
}

constexpr int len_foo_constexpr()
{
    return 5;
}

#if 1
/* 
    error: body of ‘constexpr’ function ‘constexpr int fibonacci(int)’ not a return-statement
    在C++11标准下，该函数无法编译通过，因为constexpr函数必须有且只有一条return语句
    但在C++14之后，该函数可以被编译通过，取消了该规定，只要返回字面值类型即可
*/
constexpr int fibonacci(int n)
{
    if (n == 1) return 1;
    if (n == 2) return 1;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

#endif

#if 0
constexpr int fibonacci(const int n)
{
    return n == 1 || n == 2 ? 1 : fibonacci(n - 1) + fibonacci(n - 2);
}
#endif

int main()
{
    char arr_1[10];        /* 合法 */
    char arr_2[LEN];       /* 合法 */

    int len = 10;
    /* 非法, C++标准中，数组长度必须是常量表达式，但是len是一个变量，所以实际上是一个非法行为（只是因为编译器优化了）*/
    // char arr_3[len];

    const int len_2 = len + 1;
    constexpr int len_2_constexpr = 1 + 2 + 3;
    // char arr_4[len_2];    /* 非法，len_2是一个const常数，而不是常量表达式 */
    char arr_4[len_2_constexpr];       /* 合法 */

    char arr_5[len_foo() + 5];     /* 非法 */
    char arr_6[len_foo_constexpr() + 1];

    std::cout << fibonacci(10) << std::endl;

    return 0;
}
