#include <iostream>
#include <thread>
#include <future>
#include <ranges>
// using namespace std;

#define lli long long int

lli findSum(int startIndex, int endIndex) {
    lli sum=0;
    for(const auto &val: std::views::iota(startIndex, endIndex+1)) {
        sum+=val;
    }
    return sum;
}

int main() {

    int total = 1000000;
    int start1 = 1, end1 = total/2;
    int start2 = end1+1, end2 = total;

    std::future<lli> sum1 = std::async(std::launch::async , findSum, start1, end1);
    std::future<lli> sum2 = std::async(std::launch::async, findSum, start2, end2);

    lli totalSum = sum1.get() + sum2.get();
    std::cout<<"totalSum = "<<totalSum<<"\n";

    return 0;

}