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

void pay_system(PremiumClient &client);

std::shared_mutex g_mtx;
std::vector<thread_struct> v_thread_struct;
std::queue<std::shared_ptr<request>> petitions_queue;
std::condition_variable payment_cv;

void create_threads(int id,int type, std::string word)
{
    
    if (type == 0)
    {
        FreeClient free_client = FreeClient(id, word);
        std::cout << "\033[1;36mFree client with id " << free_client.getId() << " created.\033[0m" << std::endl;
        std::cout << "\033[1;36mFree client word: " << free_client.getWord() << "\033[0m" << std::endl;
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

void searcher_system()
{
    std::cout << "Searcher system started." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void find_word_in_file(std::string file_name, int thread_v_pos)
{
    /*  Se inician y se crean las variables necesarias para la función. */
    int current_line = 0;
    std::fstream file;
    std::string line;

    /*  Se abre el fichero y se lee línea a línea.   */
    file.open(file_name.c_str());

    std::string current_word, previous_word;

    // Se convierte la palabra a mayúsculas para que no haya problemas con las mayúsculas y minúsculas.
    my_to_upper(v_thread_struct[thread_v_pos].word);

    /* Se lee el fichero línea a línea hasta que se llega a la línea final del hilo que está realizando en este momento. */
    while (getline(file, line) && current_line <= v_thread_struct[thread_v_pos].final_line)
    {
        result_struct result_str;
        if (current_line >= v_thread_struct[thread_v_pos].initial_line)
        {
            /*  Cogemos una línea y la dividimos en las palabras que tiene, vamos comprobando palabra por palabra
                si es la que estamos buscando. En caso de que sea esa, en la estructura result_struct guardamos
                la palabra anterior, la posterior y la línea donde la hemos encontrado.   */

            std::istringstream words_in_line(line);
            std::string previous_word, current_word, next_word;
            while (words_in_line >> current_word)
            {
                my_to_upper(current_word);
                if (test_each_symbol(v_thread_struct[thread_v_pos].word, current_word))
                {

                    my_to_lower(previous_word);
                    result_str.pre_word = previous_word;
                    result_str.line = current_line + 1;

                    if (words_in_line >> next_word)
                    {

                        result_str.post_word = next_word;
                    }
                    else
                    {
                        result_str.post_word = "[No hay palabra posterior]";
                    }
                    /*  Utilizamos el semáforo para añadir información en el vector de estructuras de hilos compartidos.
                        De esta manera, no acceden varios semáforos al mismo tiempo al ser una sección crítica.*/
                    std::unique_lock<std::shared_mutex> lock(g_mtx);
                    v_thread_struct[thread_v_pos].result.push(result_str);
                    lock.unlock();
                }
                previous_word = current_word;
            }
        }
        current_line++;
    }
    // Se cierra el fichero.
    file.close();
}
