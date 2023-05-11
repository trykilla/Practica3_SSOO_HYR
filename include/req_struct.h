#include <mutex>

struct request
{
    double balance;
    std::mutex mtx;
};