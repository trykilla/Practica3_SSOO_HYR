#include <mutex>

// Estructura que contiene el saldo de la cuenta y un mutex para controlar el acceso a la variable.

struct request
{
    double balance;
    std::mutex mtx;
};