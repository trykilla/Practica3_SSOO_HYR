#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>

pid_t create_process(int type, std::string word)
{
    pid_t pid;
    pid = fork();

    if (pid < 0)
    {
        std::cout << "Error: no se pudo crear proceso cliente." << std::endl;
        exit(1);
    }
    else if (pid == 0)
    {
        if (type == 0)
        {
            FreeClient free_client = FreeClient(getpid(), word);
            std::cout << "Free client with id " << free_client.getId() << " created." << std::endl;
            std::cout << "Free client word: " << free_client.getWord() << std::endl;
        }
        else if (type == 1)
        {
            PremiumClient premium_client = PremiumClient(getpid(), word, 100);
            // Código para proceso cliente de type 2
            std::cout << "Premium client with id " << premium_client.getId() << " created." << std::endl;
        }
        else if (type == 2)
        {
            ExtraPremiumClient extra_premium_client = ExtraPremiumClient(getpid(), word);
            // Código para proceso cliente de type 3
            std::cout << "Extra premium client with id " << extra_premium_client.getId() << " created." << std::endl;
        }
        else
        {
            // type inválido
            std::cout << "Error: type de proceso cliente inválido." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    
    return pid;
}
