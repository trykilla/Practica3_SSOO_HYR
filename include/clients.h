#include <iostream>
#include <thread>
#include <string>
#include <queue>
#include <req_struct.h>
#include <mutex>

class Client
{
protected:
    int id;
    std::string word;

public:
    Client(int id, std::string word)
    {
        this->word = word;
        this->id = id;
    }

    int getId() const
    {
        return id;
    }

    std::string getWord() const
    {
        return word;
    }
};

class FreeClient : public Client
{
public:
    FreeClient(int id, std::string word) : Client(id, word)
    {
    }
};

class PremiumClient : public Client
{
private:
    double balance;
    std::mutex client_mtx;

public:
    PremiumClient(int id, std::string word, double balance) : Client(id, word)
    {
        this->balance = balance;
    }

    void block_client()
    {
        client_mtx.lock();
    }

    void unblock_client()
    {
        client_mtx.unlock();
    }

    double get_balance() const
    {
        return balance;
    }

    void set_balance(double balance)
    {
        this->balance = balance;
    }
};

class ExtraPremiumClient : public Client
{
public:
    ExtraPremiumClient(int id, std::string word) : Client(id, word)
    {
        // Constructor por defecto
    }
};
