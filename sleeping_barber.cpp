#include<iostream>
#include<thread>
#include<mutex>
#include<condition_variable>

using namespace std;
const int NUM_SEATS = 5;

int waiting = 0;
std::mutex mtx;
std::condition_variable barber_cv, customer_cv;
std::mutex cout_mutex;
bool barber_sleeping=true;

int served=0; // needs to be updated in thread safe manner.
void customer(int id) {
    int my_turn;

    std::unique_lock<std::mutex> lock(mtx);
    if(waiting == NUM_SEATS) {
            std::cout << "Customer " << id << " — no seats, leaving\n";
        return;
    }
    waiting++;

    if(barber_sleeping) {
        barber_sleeping = false;
        barber_cv.notify_one();
    }
    my_turn = waiting + served;
    customer_cv.wait(lock, [&]() {return served >= my_turn; });
    std::cout << "Customer " << id << " haircut done, leaving\n";
}


void barber() {
    while(true) {
        std::unique_lock<std::mutex> lock(mtx);
        while(waiting == 0) {
            barber_sleeping = true;
            barber_cv.wait(lock);
        }
        waiting--;
        lock.unlock();
        std::cout << "Barber cutting hair...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        served+=1;
        customer_cv.notify_one();

    }
}
int main() {

    std::thread customers[10];
    std::thread barb(barber);
    for(int i=0; i<10; i++) {
        customers[i] = std::thread(customer, i);
    }


    for(int i=0; i<10; i++) {
        customers[i].join();
    }
    barb.detach();  // let barber go, all customers done

    return 0;

}