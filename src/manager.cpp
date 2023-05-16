/************************************************************************************
 * Proyecto: Práctica 3 - Sistemas Operativos II                                    *
 * Nombre del programa: manager.cpp                                                 *
 * Autores: Héctor Alberca Sánchez-Quintanar y Rubén Crespo Calcerrada              *
 * Fecha: 14/05/2023                                                                *
 * Propósito: Programa que crea y gestiona los hilos                                *
 ************************************************************************************/

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
#include <iomanip>

void create_client();
void create_dict();
void handler(int sig);

std::vector<std::thread> v_searching_threads;
std::vector<std::thread> v_client_threads;
std::unordered_set<std::string> dictionary_set;

int main()
{
    /* Se crea un diccionario al que se le añaden diversas palabras, de ahí saldrán todas las palabras
        aleatorias que se pueden elegir entre los clientes.
        A continuación se crea el hilo para el sistema de pago y los diversos clientes. */
    create_dict();

    std::cout << MAGENTA << std::setw((80 + 30) / 2) << WELCOME << RESET << std::endl;
    std::cout << MAGENTA << LINE << RESET << std::endl;
    std::cout << MAGENTA << "Número de procesadores disponibles: " << TH_NUM << RESET << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::thread payment_th(pay_system);
    payment_th.detach();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    signal(SIGINT, handler);
    books_names();
    create_client();

    std::cout << GREEN << "Todos los hilos terminados. Si se quiere salir antes del programa, pulsar Ctrl + C" << RESET << std::endl;

    return EXIT_SUCCESS;
}

/**
 * Añade en una variable global todas las palabras del archivo.
 *
 * @param No requiere parametros.
 *
 * @returns No devuelve nada.
 */

void create_dict()
{
    std::ifstream file("Files/Dictionary.txt");
    if (file.is_open())
    {
        std::string word;
        while (file >> word)
        {
            dictionary_set.insert(word);
        }
        file.close();
    }
    else
    {
        std::cout << "Error: cannot open file." << std::endl;
        exit(EXIT_FAILURE);
    }
}

/**
 * Maneja las señales de interrupción del programa.
 *
 * @param signum Número de la señal
 *
 * @returns No devuelve nada
 */
void handler(int sig)
{
    std::cout << "Señal SIGINT recibida, terminando procesos y liberando recursos..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    exit(EXIT_SUCCESS);
}

/**
 * Crea los hilos de los clientes.
 *
 * @param No requiere parametros.
 *
 * @returns No devuelve nada
 */

void create_client()
{
    /* Se calcula un aleatorio para el tipo de clientes y se elige al azar la palabra entre las del diccionario,
        con esto se crean los diversos clientes y se le asgina un hilo a cada uno de los clientes. */
    for (int i = 0; i < TH_NUM; i++)
    {
        int type = rand() % 3;
        std::string random_word = *std::next(std::begin(dictionary_set), std::rand() % dictionary_set.size());

        v_client_threads.push_back(std::thread(create_threads, i, type, random_word));
        std::this_thread::sleep_for(std::chrono::seconds(1)); // tiempo de espera para no colapsar la terminal de clientes
    }
    /* Se espera a que terminen todos los clientes. */
    std::for_each(v_client_threads.begin(), v_client_threads.end(), [](std::thread &t)
                  { t.join(); });
}
