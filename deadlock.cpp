#include<iostream>
#include<thread>
#include<mutex>

std::mutex mutexA, mutexB;


void thread1() {
#if 0
    std::lock_guard<std::mutex> lock1(mutexA);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::lock_guard<std::mutex> lock2(mutexB);
#endif
    std::scoped_lock lock(mutexA, mutexB);  // locks both atomically
    std::cout<<"Thread 1 Done\n";
}

void thread2() {
#if 0
    std::lock_guard<std::mutex> lock2(mutexB);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::lock_guard<std::mutex> lock1(mutexA);
#endif
    std::scoped_lock lock(mutexA, mutexB);  // locks both atomically
    std::cout<<"Thread 2 Done\n";

}

int main() {
    std::thread threadA(thread1);
    std::thread threadB(thread2);

    threadA.join();
    threadB.join();  
    return 0;
}