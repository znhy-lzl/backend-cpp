#include <initializer_list>

// 尝试特化std::initializer_list为int类型
template<>
class std::initializer_list<int> {
    // 特化的实现细节...
};

int main() {
    // 由于特化是不允许的，这里的代码实际上是无效的
    std::initializer_list<int> list = {1, 2, 3};
    return 0;
}
