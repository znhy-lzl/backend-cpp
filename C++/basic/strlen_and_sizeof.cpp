#include <iostream>
#include <string.h>

int main()
{
    char s1[] = "hello";
    std::cout << strlen(s1) << std::endl;
    std::cout << sizeof(s1) << std::endl;

    const char *s2 = "hello";
    std::cout << strlen(s2) << std::endl;
    std::cout << sizeof(s2) << std::endl;

    char *s3 = new char(5);
    strcpy(s3, "hello");
    std::cout << strlen(s3) << std::endl;
    std::cout << sizeof(s3) << std::endl;
    return 0;
}
