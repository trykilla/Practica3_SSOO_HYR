/************************************************************************************
 * Proyecto: Práctica 3 - Sistemas Operativos II                                    *
 * Nombre del programa: clients.h                                                   *                                                       
 * Autores: Héctor Alberca Sánchez-Quintanar y Rubén Crespo Calcerrada              *
 * Fecha: 14/05/2023                                                                *
 * Propósito: Programa dedicado a los clientes, encontramos la clase clientes,      *
 * a continuación, tenemos las 3 clases que heredan de la clase client, encontramos *
 * los 3 tipos de clientes, gratuito, premium y extra premium.                      *   
 ************************************************************************************/


#include <iostream>
#include <thread>
#include <string>
#include <queue>
#include <req_struct.h>
#include <thread_struct.h>
#include <mutex>
#include <fstream>
#include <def.h>

class Client
{
public:
    int id;
    std::string word;
    std::string client_type;
    std::vector<thread_struct> v_thread_struct;
    

public:
    Client(int id, std::string word, std::string client_type)
    {
        this->word = word;
        this->id = id;
        this->client_type = client_type;
    }

    int getId() const
    {
        return id;
    }

    std::string getWord() const
    {
        return word;
    }

    /**
    * Imprime los resultados de la búsqueda de la palabra en el fichero results.txt.
    *
    * @param word Palabra a buscar
    * @param book_name Nombre del libro en el que se busca la palabra
    *
    * @returns No devuelve nada
    */
    void print_results(std::string word, std::string book_name)
    {
        std::ofstream outfile;
        bool found_results = false;
        outfile.open("Files/Results.txt", std::ios::app);
        outfile << "-------------------------------------------------------------------------------------------------" << std::endl;
        outfile << "Tipo de cliente: " << client_type <<" Cliente " << id << " :: palabra " << word << std::endl;
        outfile << "Libro: " << book_name << "\n "<<std::endl;
        
        
        for (int i = 0; i < TH_NUM; i++)
        {
            // Se imprime el resultado de cada hilo hasta que el vector de resultados de cada hilo esté vacío.
            while (!v_thread_struct[i].result.empty())
            {
                found_results = true;
                outfile << "[Hilo " << i << " inicio:" << v_thread_struct[i].initial_line << " - final: " << v_thread_struct[i].final_line << "]";
                outfile << " :: línea " << v_thread_struct[i].result.front().line << " :: ... " << v_thread_struct[i].result.front().pre_word << " "
                        << "" << word << ""
                        << " " << v_thread_struct[i].result.front().post_word << " ..." << std::endl;
                v_thread_struct[i].result.pop();
            }
            
        }
        if (!found_results)
        {
            outfile << "No se han encontrado resultados para la palabra " << word << " en el libro " << book_name << std::endl;
        }
    }
};

class FreeClient : public Client
{

public:
    FreeClient(int id, std::string word, std::string client_type) : Client(id, word, client_type)
    {
    }
};

class PremiumClient : public Client
{
public:
    double balance;
    

public:
    PremiumClient(int id, std::string word, std::string client_type,double balance) : Client(id, word, client_type)
    {
        this->balance = balance;
    }


    double get_balance() const
    {
        return balance;
    }

    void set_balance(double balance)
    {
        this->balance = balance;
    }
};

class ExtraPremiumClient : public Client
{
public:
    ExtraPremiumClient(int id, std::string word, std::string client_type) : Client(id, word, client_type)
    {
        // Constructor por defecto
    }
};
