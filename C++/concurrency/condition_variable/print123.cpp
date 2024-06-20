#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
int num = 1;

void printNumbers(int target, int threadNum) {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [target]() { return num % 3 == target; });

        std::cout << threadNum;
        num++;
        
        cv.notify_all();  // 唤醒所有线程
    }
}

int main() {
    std::thread t1(printNumbers, 1, 1);  // 线程1打印1
    std::thread t2(printNumbers, 2, 2);  // 线程2打印2
    std::thread t3(printNumbers, 0, 3);  // 线程3打印3

    t1.join();
    t2.join();
    t3.join();

    return 0;
}
