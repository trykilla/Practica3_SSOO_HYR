
/************************************************************************************
 * Proyecto: Práctica 2 - Sistemas Operativos II                                    *
 * Nombre del programa: thread_struct.h                                             *                                                       
 * Autores: Héctor Alberca Sánchez-Quintanar y Rubén Crespo Calcerrada              *
 * Fecha: 8/04/2023                                                                *
 * Propósito: archivo con las diferentes estructuras que se van a utilizar          *
 * a lo largo de la práctica.                                                       *   
 ************************************************************************************/

#include <iostream>
#include <thread>
#include <string>
#include <queue>
#include <mutex>

#ifndef THREAD_STRUCT_H
#define THREAD_STRUCT_H

/* Se trata de la estructura de cada una de las soluciones encontradas a lo largo de la búsqueda. */
struct result_struct
{
    std::string pre_word;
    std::string post_word;
    int line;
};

/* Se trata de la estructura de cada uno de los hilos que se van a crear. Cada uno tendrá una serie 
    de estructuras de resultados, según los que haya encontrado en sus respectivas líneas. */
struct thread_struct
{
    int thread_id;
    int initial_line;
    int final_line;
    std::string word;
    std::queue<result_struct> result;

};


#endif