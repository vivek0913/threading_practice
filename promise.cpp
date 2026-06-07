#include<iostream>
#include <thread>
#include <future>

using namespace std;


void stage1(const vector<int> & data, std::promise<int> output) {
    int sum=0;
    for(const auto &i: data) { sum+=i; }
    output.set_value(sum);
}

void stage2(std::future<int> input, std::promise<int> output) {
    int value = input.get();
    output.set_value(value*value);
}
int main() {
    vector<int> data = {1, 2, 3, 4, 5};

    std::promise<int> p1,p2;
    std::future<int> f1 = p1.get_future();
    std::future<int> f2 = p2.get_future();

    std::thread t1(stage1, data, std::move(p1));
    std::thread t2(stage2, std::move(f1), std::move(p2));

    std::cout<<"Value = "<<f2.get()<<"\n";
    // optimal thread count for CPU bound work:
unsigned int cores = std::thread::hardware_concurrency();
std::cout << "cores="<<cores << "\n";  // e.g. 8 on your M-series Mac

    t1.join();
    t2.join();

    return 0;

}