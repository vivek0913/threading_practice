#include <iostream>
#include <thread>
#include <mutex>
#include <ranges>
#include <shared_mutex>
using namespace std;

uint32_t value=0;
std::shared_mutex sh_mtx;
std::mutex cout_mutex;


void print_read_val(const uint32_t &thread_id, const int &val) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout<<"Read value="<<val<<", from thread="<<thread_id<<"\n";
}
void read(const uint32_t &thread_id) {
    for(auto i:std::views::iota(0, 3)) {
        uint32_t val=-1;
        {
            std::shared_lock<std::shared_mutex> read_lock(sh_mtx);
            // val = value.load();
            val = value;
        }

        print_read_val(thread_id, val);
        std::this_thread::sleep_for(std::chrono::microseconds(50));

    }
}

void write(uint32_t thread_id) {

    for(auto i:std::views::iota(0, 3)) {
        {
            std::unique_lock<std::shared_mutex> lock1(sh_mtx);
            // value.fetch_add(1, std::memory_order_relaxed);
            value += 1;
        }

        std::this_thread::sleep_for(std::chrono::microseconds(50));
        {
            std::unique_lock<std::shared_mutex> lock2(sh_mtx);
            // value.fetch_add(1, std::memory_order_relaxed);
            value += 1;
        }
    }
}

int main() {

    std::vector<std::thread> read_threads(5);
    std::vector<std::thread> write_threads(2);

    for(auto i: std::views::iota(0, 2)) {
        write_threads[i] = std::thread(write, i);
    }

    for(auto i:std::views::iota(0, 5)) {
        read_threads[i] = std::thread(read, i);
    }

    for(auto i: std::views::iota(0, 2)) {
        write_threads[i].join();
    }

    for(auto i: std::views::iota(0, 5)) {
        read_threads[i].join();
    }

    return 0;

}