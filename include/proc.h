#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <mutex>
#include <thread>
#include <fstream>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <thread>
#include <sys/types.h>
#include <my_lib.h>
#include <shared_mutex>
#include <thread_struct.h>
#include <req_searchs.h>
#include <searcher.h>

#define WORDS_NUM 4
#define TH_NUM 4

void pay_system(PremiumClient &client);
void books_names();

std::mutex g_mtx;
std::vector<thread_struct> v_thread_struct;
std::queue<sh_request> petitions_queue;
std::condition_variable payment_cv;
std::condition_variable search_cv;
std::vector<std::thread> v_threads_books;
std::vector<FreeClient> v_free_clients;

void create_threads(int id, int type, std::string word)
{

    if (type == 0)
    {
        FreeClient free_client = FreeClient(id, word);
        std::cout << "\033[1;36mFree client with id " << free_client.getId() << " created.\033[0m" << std::endl;
        struct sh_request req;
        req.client_id = free_client.getId();
        req.client_type = type;
        req.balance = 0;
        req.word = free_client.getWord();

        petitions_queue.push(req);
        std::cout << "Peticiones " << petitions_queue.size() << std::endl;

        std::unique_lock<std::mutex> lock(g_mtx);
        search_cv.wait(lock, []
                       { return petitions_queue.size() <= 4; });
        lock.unlock();

        FreeSearcher f_searcher = FreeSearcher(id, word, WORDS_NUM);
        std::cout << "\033[1;36mFree searcher with id " << f_searcher.getId() << " created.\033[0m" << std::endl;
        for (int i = 0; i < books_names_vector.size() && f_searcher.word_counter <= WORDS_NUM; i++)
        {
            // std::cout << books_names_vector[i] << std::endl;
            f_searcher.new_search(books_names_vector[i], word);

            std::cout << "No hay palabra en ese libro" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }
        std::cout << "\033[35mFree searcher with id " << f_searcher.getId() << " finished.\033[0m" << std::endl;

        for (int i = 0; i < f_searcher.v_thread_struct.size(); i++)
        {
            free_client.v_thread_struct.push_back(f_searcher.v_thread_struct[i]);
        }

        v_free_clients.push_back(free_client);
        std::cout << "Peticiones " << petitions_queue.size() << std::endl;
        petitions_queue.pop();
        search_cv.notify_one();
        free_client.print_results(word);
    }
    else if (type == 1)
    {
        srand(time(NULL));
        int balance = rand() % 2;
        PremiumClient premium_client = PremiumClient(id, word, balance);

        std::cout << "\033[1;31mPremium client with id " << premium_client.getId() << " created.\033[0m" << std::endl;
        std::cout << "\033[1;31mPremium client word: " << premium_client.getWord() << "\033[0m" << std::endl;
        std::cout << "\033[1;31mPremium client balance: " << premium_client.get_balance() << "\033[0m" << std::endl;
    }
    else if (type == 2)
    {
        ExtraPremiumClient extra_premium_client = ExtraPremiumClient(id, word);
        std::cout << "\033[33mExtra premium client with id " << extra_premium_client.getId() << " created.\033[0m" << std::endl;
        std::cout << "\033[33mExtra premium client word: " << extra_premium_client.getWord() << "\033[0m" << std::endl;
    }
    
}

void pay_system(PremiumClient &client)
{
    std::cout << "Paying..." << std::endl;
    client.set_balance(100);
    std::cout << "Payment done." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

