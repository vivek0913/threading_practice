#include<iostream>
#include<thread>
#include<queue>

using namespace std;

queue<function<void(uint32_t)>> tasks;
std::mutex mtx;
std::condition_variable cv;
bool done = false;
std::mutex cout_mtx;
std::atomic<int> prod_count(0);
auto task = [](uint32_t id) {
    cout<<"Hello from consumer thread: "<<id<<"\n";
};

void producer(uint32_t id) {

    for(int i=0; i<4; i++) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            tasks.push(task);
        }
        cv.notify_one();
        {
            std::lock_guard<std::mutex> lock(cout_mtx);
            std::cout<<"Pushed task="<<i<<" into task queue from producer thread="<<id<<"!\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    if(++prod_count == 4) {
    
    std::unique_lock<std::mutex> lock(mtx);
    done=true;
    cv.notify_all();
    }

}
void consume(uint32_t id) {
    while(true) {
            function<void(uint32_t)> task;
            {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, []{return !tasks.empty() || done;});
            if(done && tasks.empty()) {
                break;
            }
            task = tasks.front();
            tasks.pop();
            }
        {
            std::lock_guard<std::mutex> lock(cout_mtx);
            task(id);
        }
    }

}
int main() {

    vector<std::thread> prod_threads(4);

    for(int i=0; i<4; i++) {
        prod_threads[i] = std::thread(producer, i);
    }


    vector<std::thread> threads(4);
    for(int i=0; i<4; i++) {
        threads[i] = std::thread(consume, i);
    }

    for(int i=0; i<4; i++) {
        prod_threads[i].join();
    }
    for(int i=0; i<4; i++) {
        threads[i].join();
    }

    return 0;

}