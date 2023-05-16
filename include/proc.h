/************************************************************************************
 * Proyecto: Práctica 3 - Sistemas Operativos II                                    *
 * Nombre del programa: proc.h                                                      *
 * Autores: Héctor Alberca Sánchez-Quintanar y Rubén Crespo Calcerrada              *
 * Fecha: 14/05/2023                                                                *
 * Propósito: archivo en el cual se encuentran las distintas funciones              *
 * que van a ser utilizadas para la creación y la gestión de los clientes.          *
 * En este archivo también se llamarán a los buscadores.                            *
 ************************************************************************************/


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


/* Declaramos todas las funciones y las variables globales necesarias en el código.*/
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

    /* Dependiendo del tipo de cliente que se haya creado debemos hacer una busqueda u otra. */
    if (type == 0)
    {
        //Creamos el cliente, en este caso el gratis y creamos una peticion para la cola.
        //Esta petición tendra un 20% de posibilidades de ser insertada en la cola.

        FreeClient free_client = FreeClient(id, word, "Free Client");
        std::cout << BLUE << "Free client with id " << free_client.getId() << " created." << RESET << std::endl;

        struct sh_request req;
        create_request(free_client, type, 0, req);
        insert_in_queue(req, 0, 20);

        //Se lleva a cabo la busqueda de las palabras en los libros.
        //Siempre y cuando haya menos de 4 clientes realizando busquedas. 

        FreeSearcher f_searcher = FreeSearcher(id, word, type, WORDS_NUM);
        f_searcher.credit_counter = 1;

        searchers_wait();

        std::cout << BLUE << "Searcher with id " << f_searcher.getId() << " created." << RESET << std::endl;

        //Se mide el tiempo que tarda en realizar la busqueda.
        auto start_time = std::chrono::high_resolution_clock::now();
        search_for_words(f_searcher, free_client, word);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        std::cout << BLUE << "Client with id " << free_client.getId() << " finished in " << elapsed_time_ms << " ms." << RESET << std::endl;
        std::cout << BLUE << "Searcher with id " << f_searcher.getId() << " finished." << RESET << std::endl;

        //Se añade el cliente a un vector de clientes gratuitos y se elimina la petición de la cola.
        //Además, se notifica a los clientes que estaban esperando a que se eliminara la petición y un buscador quedará libre.
        v_free_clients.push_back(free_client);
        petitions_queue.pop();
        search_cv.notify_one();
    }
    else if (type == 1)
    {
        //Creamos el cliente, en este caso el premium y creamos una peticion para la cola.
        //Creamos un aleatorio que va a ser el saldo que va a tener el cliente, después hará la petición de pago. 

        srand(time(NULL));
        int balance = rand() % 101;

        PremiumClient premium_client = PremiumClient(id, word, "Premium Client", balance);
        std::cout << YELLOW << "Premium client with id " << premium_client.getId() << " created." << RESET << std::endl;

        //Se crea la petición y se inserta en la cola.
        struct sh_request req;
        create_request(premium_client, type, balance, req);
        insert_in_queue(req, 20, 100);

        searchers_wait();

        //Se le asigna el buscador y se le asigna el saldo que tiene el cliente.
        FreeSearcher f_searcher = FreeSearcher(id, word, type, WORDS_NUM);
        f_searcher.credit_counter = premium_client.get_balance();
        std::cout << YELLOW << "Searcher with id " << f_searcher.getId() << " created." << RESET << std::endl;

        auto start_time = std::chrono::high_resolution_clock::now();
        search_for_words(f_searcher, premium_client, word);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        std::cout << YELLOW << "Client with id " << premium_client.getId() << " finished in " << elapsed_time_ms << " ms." << RESET << std::endl;

        //Se le asigna el saldo que le queda al cliente.
        premium_client.set_balance(f_searcher.credit_counter);

        std::cout << YELLOW << "Searcher with id " << f_searcher.getId() << " finished." << RESET << std::endl;

        //Se inserta el cliente en un vector con todos los clientes premium y se le saca de la cola de peticones.
        //Además, se notifica a los clientes que estaban esperando a que se eliminara la petición y un buscador quedará libre.
        v_premium_clients.push_back(premium_client);
        petitions_queue.pop();
        search_cv.notify_one();
    }
    else if (type == 2)
    {
        //Creamos el cliente, en este caso el extra premium y creamos una peticion para la cola.
        //Este cliente no tendrá saldo, ya que se le devolverán todas las palabras encontradas. 

        ExtraPremiumClient extra_premium_client = ExtraPremiumClient(id, word, "Extra Premium Client");
        std::cout << GREEN << "Extra premium client with id " << extra_premium_client.getId() << " created." << RESET << std::endl;

        //Se crea la petición y se inserta en la cola.
        struct sh_request req;
        create_request(extra_premium_client, type, 0, req);
        insert_in_queue(req, 20, 100);

        searchers_wait();
        //Se le asigna el buscador, siempre y cuando haya menos de 4 clientes buscando.
        FreeSearcher f_searcher = FreeSearcher(id, word, type, WORDS_NUM);
        f_searcher.credit_counter = 0;

        std::cout << GREEN << "Searcher with id " << f_searcher.getId() << " created." << RESET << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();
        search_for_words(f_searcher, extra_premium_client, word);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        std::cout << GREEN << "Client with id " << extra_premium_client.getId() << " finished in " << elapsed_time_ms << " ms." << RESET << std::endl;
        std::cout << GREEN << "Searcher with id " << f_searcher.getId() << " finished." << RESET << std::endl;

        //Se inserta el cliente en un vector con todos los clientes extra premium y se le saca de la cola de peticones.
        //Además, se notifica a los clientes que estaban esperando a que se eliminara la petición y un buscador quedará libre.
        v_extra_premium_clients.push_back(extra_premium_client);
        petitions_queue.pop();
        search_cv.notify_one();
    }
}

/**
 * Hace la llamada al método del buscador para que busque en todos los libros y escriba los resultados.
 *
 * @param searcher Buscador que va a realizar la búsqueda
 * @param client Cliente que va a recibir los resultados
 * @param word Palabra que se va a buscar
 *
 * @returns No devuelve nada
 */

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

/**
 * Crea las peticiones de los clientes 
 *
 * @param client Cliente que va a realizar la petición
 * @param type Tipo de cliente que va a realizar la petición
 * @param balance Saldo que tiene el cliente
 * @param req Petición que se va a crear
 *
 * @returns No devuelve nada
 */

void create_request(Client client, int type, int balance, sh_request &req)
{
    req.client_id = client.getId();
    req.client_type = type;
    req.balance = balance;
    req.word = client.getWord();
}

/**
 * Inserta la petición en la cola de peticiones
 *
 * @param req Petición que se va a insertar
 * @param inf_lim Límite inferior del rango de números aleatorios
 * @param sup_lim Límite superior del rango de números aleatorios
 *
 * @returns No devuelve nada
 */

void insert_in_queue(sh_request req, int inf_lim, int sup_lim)
{
    srand(time(NULL));

    //Dependiendo del tipo de cliente que sea tendrá una probabilidad u otra de ser insertado en la cola.
    //Hasta que no salga un número aleatorio que esté dentro del rango no se insertará en la cola.
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

/**
 * Espera a los buscadores, si hay más de 4 buscando, se queda esperando.
 *
 * @param No requiere parámetros
 *
 * @returns No devuelve nada
 */

void searchers_wait()
{
    std::unique_lock<std::mutex> lock(g_mtx);

    search_cv.wait(lock, []
                   { return petitions_queue.size() <= 4; });

    lock.unlock();
}