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
#include <searcher.h>

#define WORDS_NUM 4
#define TH_NUM 4

void pay_system(PremiumClient &client);
void books_names();
void print_results(FreeSearcher &searcher, std::string word);

std::shared_mutex g_mtx;
std::vector<thread_struct> v_thread_struct;
std::queue<std::shared_ptr<request>> petitions_queue;
std::condition_variable payment_cv;
std::vector<std::thread> v_threads_books;

void create_threads(int id, int type, std::string word)
{

    if (type == 0)
    {
        FreeClient free_client = FreeClient(id, word);
        std::cout << "\033[1;36mFree client with id " << free_client.getId() << " created.\033[0m" << std::endl;
        std::cout << "\033[1;36mFree client word: " << free_client.getWord() << "\033[0m" << std::endl;
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
        print_results(f_searcher, word);
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

void print_results(FreeSearcher &f_searcher, std::string word)
{
    for (int i = 0; i < TH_NUM; i++)
    {
        // Se imprime el resultado de cada hilo hasta que el vector de resultados de cada hilo esté vacío.
        while (!f_searcher.v_thread_struct[i].result.empty())
        {
            std::cout << "[Hilo " << i << " inicio:" << f_searcher.v_thread_struct[i].initial_line << " - final: " << f_searcher.v_thread_struct[i].final_line << "]";
            std::cout << " :: línea " << f_searcher.v_thread_struct[i].result.front().line << " :: ... " << f_searcher.v_thread_struct[i].result.front().pre_word << " "
                      << "\x1B[3m" << word << "\x1B[0m"
                      << " " << f_searcher.v_thread_struct[i].result.front().post_word << " ..." << std::endl;
            f_searcher.v_thread_struct[i].result.pop();
        }
    }
}