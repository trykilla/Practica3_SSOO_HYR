#include <iostream>
#include <thread>
#include <string>
#include <queue>
#include <req_struct.h>
#include <thread_struct.h>
#include <mutex>
#include <fstream>

#define TH_NUM 4

class Client
{
public:
    int id;
    std::string word;
    std::vector<thread_struct> v_thread_struct;

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

    void print_results(std::string word, std::string book_name)
    {
        std::ofstream outfile;
        outfile.open("Results.txt", std::ios::app);
        outfile << "Cliente " << id << " :: palabra " << word << std::endl;
        outfile << "Libro: " << book_name << std::endl;
        
        for (int i = 0; i < TH_NUM; i++)
        {
            // Se imprime el resultado de cada hilo hasta que el vector de resultados de cada hilo esté vacío.
            while (!v_thread_struct[i].result.empty())
            {
                outfile << "[Hilo " << i << " inicio:" << v_thread_struct[i].initial_line << " - final: " << v_thread_struct[i].final_line << "]";
                outfile << " :: línea " << v_thread_struct[i].result.front().line << " :: ... " << v_thread_struct[i].result.front().pre_word << " "
                        << "" << word << ""
                        << " " << v_thread_struct[i].result.front().post_word << " ..." << std::endl;
                v_thread_struct[i].result.pop();
            }
            outfile << "\n"
                    << std::endl;
        }
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
