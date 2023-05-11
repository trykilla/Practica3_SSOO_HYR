#include <iostream>
#include <clients.h>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <proc.h>
#include <def.h>


void create_client(int type);

void handler(int sig);
// void wake_up_searchers();



std::vector<std::string> dictionary = {"david"};
std::vector<std::thread> v_searching_threads;
std::vector<std::thread> v_client_threads;

int main()
{
    std::thread payment_th(pay_system);
    srand(time(NULL));
    int type = 2;
    signal(SIGINT, handler);
    books_names();
    create_client(type);
    // wake_up_searchers();
    std::cout << "Todos los procesos hijos creados." << std::endl;
    std::cout << "Todos los procesos hijos terminados." << std::endl;
    payment_th.join();
    
    return EXIT_SUCCESS;
}

void handler(int sig)
{
    std::cout << "Señal SIGINT recibida, terminando procesos y liberando recursos..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    exit(EXIT_SUCCESS);
}

void create_client(int type)
{
    for (int i = 0; i < 1; i++)
    {
        type = rand() % 3;

        v_client_threads.push_back(std::thread(create_threads, i, 2, dictionary[rand() % dictionary.size()]));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::for_each(v_client_threads.begin(), v_client_threads.end(), [](std::thread &t)
                  { t.join(); });
}
