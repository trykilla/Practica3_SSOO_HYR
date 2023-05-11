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
#include <def.h>

void pay_system();
void books_names();

std::mutex g_mtx;

std::vector<thread_struct> v_thread_struct;
std::queue<sh_request> petitions_queue;

std::condition_variable search_cv;
std::vector<std::thread> v_threads_books;
std::vector<FreeClient> v_free_clients;
std::vector<PremiumClient> v_premium_clients;
std::vector<ExtraPremiumClient> v_extra_premium_clients;

void any_client(Client client);

void create_threads(int id, int type, std::string word)
{

    if (type == 0)
    {
        FreeClient free_client = FreeClient(id, word, "Free Client");
        std::cout << "\033[1;36mFree client with id " << free_client.getId() << " created.\033[0m" << std::endl;
        struct sh_request req;
        req.client_id = free_client.getId();
        req.client_type = type;
        req.balance = 0;
        req.word = free_client.getWord();

        while (true)
        {
            srand(time(NULL));
            int in = rand() % 101;
            if (in >= 0 && in <= 20)
            {
                petitions_queue.push(req);
                break;
            }
        }
        std::cout << "Peticiones " << petitions_queue.size() << std::endl;

        std::unique_lock<std::mutex> lock(g_mtx);
        search_cv.wait(lock, []
                       { return petitions_queue.size() <= 4; });
        lock.unlock();

        FreeSearcher f_searcher = FreeSearcher(id, word, type, WORDS_NUM);
        f_searcher.credit_counter = 1;
        std::cout << "\033[1;36mFree searcher with id " << f_searcher.getId() << " created.\033[0m" << std::endl;
        for (int i = 0; i < books_names_vector.size() && f_searcher.word_counter < WORDS_NUM; i++)
        {
            std::cout << "\033[35mFree searcher with id " << f_searcher.getId() << " searching in book " << books_names_vector[i] << "\033[0m" << std::endl;
            f_searcher.new_search(books_names_vector[i], word, free_client.v_thread_struct);

            free_client.print_results(word, books_names_vector[i]);

            free_client.v_thread_struct.clear();
        }
        std::cout << "\033[35mFree searcher with id " << f_searcher.getId() << " finished.\033[0m" << std::endl;

        v_free_clients.push_back(free_client);
        std::cout << "Peticiones " << petitions_queue.size() << std::endl;
        petitions_queue.pop();
        search_cv.notify_one();
    }
    else if (type == 1)
    {
        srand(time(NULL));
        // // int balance = rand() % 2;
        int balance = 5;
        PremiumClient premium_client = PremiumClient(id, word, "Premium Client", balance);
        struct sh_request req;
        req.client_id = premium_client.getId();
        req.client_type = type;
        req.balance = balance;
        req.word = premium_client.getWord();

        while (true)
        {
            srand(time(NULL));
            int in = rand() % 101;
            if (in >= 20 && in <= 100)
            {
                petitions_queue.push(req);
                break;
            }
        }

        std::cout << "Peticiones " << petitions_queue.size() << std::endl;

        std::unique_lock<std::mutex> p_lock(g_mtx);
        search_cv.wait(p_lock, []
                       { return petitions_queue.size() <= 4; });
        p_lock.unlock();

        FreeSearcher f_searcher = FreeSearcher(id, word, type, WORDS_NUM);
        f_searcher.credit_counter = premium_client.get_balance();
        std::cout << "\033[1;36mFree searcher with id " << f_searcher.getId() << " created.\033[0m" << std::endl;
        for (int i = 0; i < books_names_vector.size() && f_searcher.word_counter < WORDS_NUM; i++)
        {
            std::cout << "\033[35mFree searcher with id " << f_searcher.getId() << " searching in book " << books_names_vector[i] << "\033[0m" << std::endl;
            f_searcher.new_search(books_names_vector[i], word, premium_client.v_thread_struct);
            premium_client.print_results(word, books_names_vector[i]);
            premium_client.v_thread_struct.clear();
        }
        premium_client.set_balance(f_searcher.credit_counter);
        std::cout << "Balance final: " << premium_client.get_balance() << std::endl;
        std::cout << "\033[35mFree searcher with id " << f_searcher.getId() << " finished.\033[0m" << std::endl;

        v_premium_clients.push_back(premium_client);
        std::cout << "Peticiones " << petitions_queue.size() << std::endl;
        petitions_queue.pop();
        search_cv.notify_one();
        std::cout << "\033[1;31mPremium client with id " << premium_client.getId() << " created.\033[0m" << std::endl;
        std::cout << "\033[1;31mPremium client word: " << premium_client.getWord() << "\033[0m" << std::endl;
        std::cout << "\033[1;31mPremium client balance: " << premium_client.get_balance() << "\033[0m" << std::endl;
    }
    else if (type == 2)
    {
        ExtraPremiumClient extra_premium_client = ExtraPremiumClient(id, word, "Extra Premium Client");
        std::cout << "\033[33mExtra premium client with id " << extra_premium_client.getId() << " created.\033[0m" << std::endl;
        std::cout << "\033[33mExtra premium client word: " << extra_premium_client.getWord() << "\033[0m" << std::endl;

        struct sh_request req;
        req.client_id = extra_premium_client.getId();
        req.client_type = type;
        req.balance = 0;
        req.word = extra_premium_client.getWord();

        while (true)
        {
            srand(time(NULL));
            int in = rand() % 101;
            if (in >= 20 && in <= 100)
            {
                petitions_queue.push(req);
                break;
            }
        }

        std::unique_lock<std::mutex> lock(g_mtx);
        search_cv.wait(lock, []
                       { return petitions_queue.size() <= 4; });
        lock.unlock();

        FreeSearcher f_searcher = FreeSearcher(id, word, type, WORDS_NUM);
        f_searcher.credit_counter = 1;
        any_client(extra_premium_client);

        std::cout << "\033[1;36mExtraPremium searcher with id " << f_searcher.getId() << " created.\033[0m" << std::endl;
        for (int i = 0; i < books_names_vector.size() && f_searcher.word_counter < WORDS_NUM; i++)
        {
            std::cout << "\033[35mExtraPremium searcher with id " << f_searcher.getId() << " searching in book " << books_names_vector[i] << "\033[0m" << std::endl;
            f_searcher.new_search(books_names_vector[i], word, extra_premium_client.v_thread_struct);

            extra_premium_client.print_results(word, books_names_vector[i]);

            extra_premium_client.v_thread_struct.clear();
        }
        std::cout << "\033[35mFree searcher with id " << f_searcher.getId() << " finished.\033[0m" << std::endl;

        v_extra_premium_clients.push_back(extra_premium_client);
        std::cout << "Peticiones " << petitions_queue.size() << std::endl;
        petitions_queue.pop();
        search_cv.notify_one();
    }
}

void any_client(Client client)
{

    std::cout << "\033[1;31mClient with id " << client.getId() << " created.\033[0m" << std::endl;
    std::cout << "\033[1;31mClient word: " << client.getWord() << "\033[0m" << std::endl;
    
}
