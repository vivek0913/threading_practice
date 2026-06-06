#include <iostream>
#include<thread>
#include <ranges>
#include<mutex>
using namespace std;
std::mutex cout_mutex;
template<typename T>
class LockFreeStack {

    struct Node {
        T data;
        Node* next;
        Node(T val) : data(val), next(nullptr) {}
    };

    std::atomic<Node*> head;
    
public:
    LockFreeStack() : head(nullptr){}

    void push(T value) {
        Node* newVal = new Node(value);
        newVal->next = head.load();   // initial load
        while(!head.compare_exchange_weak(
            newVal->next,             // ← CAS updates this on failure
            newVal,
            std::memory_order_release,
            std::memory_order_relaxed));
    }

    bool pop(T &result) {
        Node* curHead = head.load(std::memory_order_acquire);
        while(curHead!=nullptr) {

            if(head.compare_exchange_weak(curHead, curHead->next, std::memory_order_release, std::memory_order_relaxed)) {
                result = curHead->data;
                delete curHead;
                return true;
            }

        }
        return false;
    }
};


int main() {
    LockFreeStack<int> st;
    vector<std::thread> writers(4), readers(4);
    for(auto i: std::views::iota(0, 4)) {
        writers[i] = std::thread([&st, i]() {
            for(int j=0; j<5; j++) {
                st.push(i*10 + j);
            }
        });
    }

    for(auto i: std::views::iota(0, 4)) {
        int value;
        readers[i] = std::thread([&st, i]() {
            int value;
            for(auto j: std::views::iota(0, 5)) {
                if(st.pop(value)) {
                    std::lock_guard<std::mutex> lock(cout_mutex);
                    std::cout<<"Popped value="<<value<<", from thread="<<i<<"\n";
                }
            }
        });
    }

    for(auto i: std::views::iota(0, 4)) {
        writers[i].join();
    }

    for(auto i: std::views::iota(0, 4)) {
        readers[i].join();
    }

    return 0;

}