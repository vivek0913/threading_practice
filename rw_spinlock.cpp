#include<iostream>
#include<thread>
#include<mutex>

using namespace std;

std::atomic<uint32_t> value(0);
std::mutex cout_mutex;
std::atomic<uint32_t> prod_done(0);
const int NUM_PRODUCERS = 5;

void producer(const int &thread_id) {

    value.fetch_add(1, std::memory_order_relaxed);
    prod_done.fetch_add(1, std::memory_order_release);

}

void consumer(const int &thread_id) {
    
    while(prod_done.load(std::memory_order_acquire) < NUM_PRODUCERS){};
    {
        std::lock_guard<std::mutex> mt(cout_mutex);
        std::cout<<"Value read = "<<value.load()<<" from thread="<<thread_id<<"\n";
    }
}
int main() {


    vector<std::thread> reads(3);
    vector<std::thread> writes(5);

    for(int i=0; i<5; i++) {
        writes[i] = std::thread(producer, i);
    }
    for(int i=0; i<3; i++) {
        reads[i] = std::thread(consumer, i);
    }

    for(int i=0; i<5; i++) {
        writes[i].join();
    }
    for(int i=0; i<3; i++) {
        reads[i].join();
    }

    return 0;
}