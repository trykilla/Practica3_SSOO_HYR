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
void searchers_wait();
void create_request(Client client, int type, int balance, sh_request &req);
void insert_in_queue(sh_request req, int inf_lim, int sup_lim);
void search_for_words(FreeSearcher &searcher, Client client, std::string word);

std::mutex g_mtx;
std::queue<sh_request> petitions_queue;
std::condition_variable search_cv;
std::vector<FreeClient> v_free_clients;
std::vector<PremiumClient> v_premium_clients;
std::vector<ExtraPremiumClient> v_extra_premium_clients;
bool continue_waiting = true;

void create_threads(int id, int type, std::string word)
{
    std::cout << MAGENTA << "Word to look for: " << word << RESET << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    if (type == 0)
    {

        FreeClient free_client = FreeClient(id, word, "Free Client");
        std::cout << BLUE << "Free client with id " << free_client.getId() << " created." << RESET << std::endl;

        struct sh_request req;
        create_request(free_client, type, 0, req);
        insert_in_queue(req, 0, 20);

        FreeSearcher f_searcher = FreeSearcher(id, word, type, WORDS_NUM);
        f_searcher.credit_counter = 1;

        searchers_wait();

        std::cout << BLUE << "Searcher with id " << f_searcher.getId() << " created." << RESET << std::endl;

        auto start_time = std::chrono::high_resolution_clock::now();
        search_for_words(f_searcher, free_client, word);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        std::cout << BLUE << "Client with id " << free_client.getId() << " finished in " << elapsed_time_ms << " ms." << RESET << std::endl;
        std::cout << BLUE << "Searcher with id " << f_searcher.getId() << " finished." << RESET << std::endl;

        v_free_clients.push_back(free_client);
        petitions_queue.pop();
        search_cv.notify_one();
    }
    else if (type == 1)
    {
        srand(time(NULL));
        int balance = rand() % 101;

        PremiumClient premium_client = PremiumClient(id, word, "Premium Client", balance);
        std::cout << YELLOW << "Premium client with id " << premium_client.getId() << " created." << RESET << std::endl;

        struct sh_request req;
        create_request(premium_client, type, balance, req);
        insert_in_queue(req, 20, 100);

        searchers_wait();

        FreeSearcher f_searcher = FreeSearcher(id, word, type, WORDS_NUM);
        f_searcher.credit_counter = premium_client.get_balance();
        std::cout << YELLOW << "Searcher with id " << f_searcher.getId() << " created." << RESET << std::endl;

        auto start_time = std::chrono::high_resolution_clock::now();
        search_for_words(f_searcher, premium_client, word);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        std::cout << YELLOW << "Client with id " << premium_client.getId() << " finished in " << elapsed_time_ms << " ms." << RESET << std::endl;

        premium_client.set_balance(f_searcher.credit_counter);

        std::cout << YELLOW << "Searcher with id " << f_searcher.getId() << " finished." << RESET << std::endl;

        v_premium_clients.push_back(premium_client);
        petitions_queue.pop();
        search_cv.notify_one();
    }
    else if (type == 2)
    {
        ExtraPremiumClient extra_premium_client = ExtraPremiumClient(id, word, "Extra Premium Client");
        std::cout << GREEN << "Extra premium client with id " << extra_premium_client.getId() << " created." << RESET << std::endl;

        struct sh_request req;
        create_request(extra_premium_client, type, 0, req);
        insert_in_queue(req, 20, 100);

        searchers_wait();

        FreeSearcher f_searcher = FreeSearcher(id, word, type, WORDS_NUM);
        f_searcher.credit_counter = 0;

        std::cout << GREEN << "Searcher with id " << f_searcher.getId() << " created." << RESET << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        search_for_words(f_searcher, extra_premium_client, word);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        std::cout << GREEN << "Client with id " << extra_premium_client.getId() << " finished in " << elapsed_time_ms << " ms." << RESET << std::endl;
        std::cout << GREEN << "Searcher with id " << f_searcher.getId() << " finished." << RESET << std::endl;

        v_extra_premium_clients.push_back(extra_premium_client);
        petitions_queue.pop();
        search_cv.notify_one();
    }
}

void search_for_words(FreeSearcher &searcher, Client client, std::string word)
{

    for (int i = 0; i < books_names_vector.size() && searcher.word_counter < WORDS_NUM; i++)
    {
        std::cout << CYAN << "Searcher with id " << searcher.getId() << " searching in book " << books_names_vector[i] << RESET << std::endl;
        searcher.new_search(books_names_vector[i], word, client.v_thread_struct);

        client.print_results(word, books_names_vector[i]);
        client.v_thread_struct.clear();
    }
}

void create_request(Client client, int type, int balance, sh_request &req)
{
    req.client_id = client.getId();
    req.client_type = type;
    req.balance = balance;
    req.word = client.getWord();
}

void insert_in_queue(sh_request req, int inf_lim, int sup_lim)
{
    srand(time(NULL));

    while (true)
    {
        int in = rand() % 101;

        if (in >= inf_lim && in <= sup_lim)
        {

            petitions_queue.push(req);
            break;
        }
    }
}

void searchers_wait()
{
    std::unique_lock<std::mutex> lock(g_mtx);

    search_cv.wait(lock, []
                   { return petitions_queue.size() <= 4; });

    lock.unlock();
}