#include<iostream>
#include<thread>
#include<mutex>

std::queue<int> q;
std::mutex mtx;
std::condition_variable cv;

bool done=false;
void producer() {
    for(int i=0; i<20; i++) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            q.push(i);
            std::cout<<"Produced: "<<i<<"\n";  
        }  
        cv.notify_one();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::unique_lock<std::mutex> lock(mtx);
    done=true;
    cv.notify_one();


}

void consumer() {
    while(true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []{return !q.empty() || done;});
        if(done && q.empty()) {
            break;
        }
        int item=q.front();
        q.pop();
        std::cout<<"Consumed: "<<item<<"\n";
    }

}

int main() {

    std::thread prod(producer);
    std::thread cons(consumer);

    prod.join();
    cons.join();

}