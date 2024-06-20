#include <iostream>

// 虚基类 Animal
class Animal {
public:
    virtual void speak() {
        std::cout << "Animal speaking" << std::endl;
    }

    void eat() {
        std::cout << "Animal eating" << std::endl;
    } 
};

// 虚继承自 Animal 的两个派生类
class Dog : virtual public Animal {
public:
    void speak() override {
        std::cout << "Dog barking" << std::endl;
    }
};

class Cat : virtual public Animal {
public:
    void speak() override {
        std::cout << "Cat meowing" << std::endl;
    }
};

// 派生类 Pet 继承自 Dog 和 Cat
class Pet : public Dog, public Cat {
public:
    void speak() override {
        std::cout << "Pet speaking" << std::endl;
    }
};

int main() {
    Pet pet;
    // pet.speak(); // 当Pet类不重写speak函数时，会编译报错，歧义
    pet.eat();
    
    return 0;
}
