#include <iostream>
#include <clients.h>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <proc.h>

void wait_process();
void create_client(int type);

#define MAX_CLIENTS 5

std::vector<std::string> dictionary = {"David", "Como", "llamo"};
pid_t g_clients_pids[MAX_CLIENTS];

int main()
{
    srand(time(NULL));
    int type = 0;

    create_client(type);

    std::cout << "Todos los procesos hijos creados." << std::endl;
    wait_process();
    std::cout << "Todos los procesos hijos terminados." << std::endl;

    return EXIT_SUCCESS;
}

void kill_process()
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        std::cout << "Matar proceso hijo " << g_clients_pids[i] << std::endl;
        if (kill(g_clients_pids[i], SIGKILL) == -1)
        {
            perror("Error al matar proceso hijo");
            exit(EXIT_FAILURE);
        }
    }
}

void wait_process()
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        std::cout << "Esperando a que termine el proceso hijo " << g_clients_pids[i] << std::endl;
        waitpid(g_clients_pids[i], NULL, 0);
    }
}

void create_client(int type)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        type = rand() % 3;
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("Error al crear proceso hijo");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            std::string word = dictionary[rand() % dictionary.size()];
            create_process(type, word);
            exit(EXIT_SUCCESS);
        }
        else
        {
            // Código del proceso padre
            g_clients_pids[i] = pid;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}
