#include <mutex>

struct request
{
    int id;
    double saldo;
    double cantidad;
    std::mutex semaforo;
};