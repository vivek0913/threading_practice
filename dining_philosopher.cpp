#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

const int N = 5;
std::mutex mutexes[N];
std::mutex cout_mutex;
void philosopher(int id) {

    int left = id;
    int right = (id + 1)%N;

    while(true) {

        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Philosopher " << id << " is thinking\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));

        {

            // std::scoped_lock lock(mutexes[left], mutexes[right]);

            int minIndex = min(left, right), maxIndex = max(left, right);

            std::lock_guard<std::mutex> lock1(mutexes[minIndex]);
            std::lock_guard<std::mutex> lock2(mutexes[maxIndex]);


            /*
            std::lock_guard<std::mutex> lock1(mutexes[left]);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            std::lock_guard<std::mutex> lock2(mutexes[right]);
            */


            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "Philosopher " << id << " is eating\n";
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
    }

}

int main() {

    vector<std::thread> philosophers(5);

    for(int i=0; i<5; i++) {
        philosophers[i] = std::thread(philosopher, i);
    }

    for(int i=0; i<5; i++) {
        philosophers[i].join();
    }

    return 0;
}