/************************************************************************************
 * Proyecto: Práctica 2 - Sistemas Operativos II                                    *
 * Nombre del programa: my_lib.h                                                    *                                                       
 * Autores: Héctor Alberca Sánchez-Quintanar y Rubén Crespo Calcerrada              *
 * Fecha: 8/04/2023                                                                 *
 * Propósito: archivo en el cual se encuentran las distintas funciones              *
 * que son genéricas y pueden ser utilizadas durante distintas prácticas.           *                                    
 ************************************************************************************/

#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <fstream>
#include <ctype.h>

int count_lines_in_file(std::string file_name);
void my_to_upper(std::string &word);
void my_to_lower(std::string &word);
bool test_each_symbol(std::string word, std::string current_word);

/**
 * Verifica que los argumentos introducidos por el usuario son correctos.
 *
 * @param argc Número de argumentos introducidos por el usuario
 *
 * @returns No devuelve nada
 */

void parse_args(int argc)
{
    if (argc != 4)
    {
        std::cerr << "[!] Error in args line. Usage: ./main <file_name> <word> <threads_number> " << std::endl;
        exit(EXIT_FAILURE);
    }
}

/**
 * Cuenta el número de líneas que tiene un fichero.
 *
 * @param file_name Nombre del fichero del que se quiere contar las líneas
 *
 * @returns Número de líneas del fichero
 */

int count_lines_in_file(std::string file_name)
{
    std::string line;
    std::ifstream file(file_name);
    int lines_num = 0;

    //Se lee el archivo línea a línea y se cuenta el número de líneas hasta el final. 
    if (file.is_open())
    {
        while (!file.eof())
        {
            getline(file, line);
            lines_num++;
        }
        file.close();
    }
    return lines_num;
}

/**
 * Comprueba si una palabra está contenida en otra.
 *
 * @param word Palabra que se quiere buscar
 * @param current_word Palabra en la que se quiere buscar
 *
 * @returns True si la palabra está contenida en la otra, false en caso contrario
 */

bool test_each_symbol(std::string word, std::string current_word)
{
    //Se compara la palabra con la palabra actual y con la palabra actual con un punto, una coma o un punto y coma al final.
    bool flag = false;
    if (current_word == word || current_word == word + "." || current_word == word + "," || current_word == word + ";")
    {
        flag = true;
    }
    return flag;
}

/**
 * Convierte una palabra a mayúsculas.
 *
 * @param word Palabra que se quiere convertir a mayúsculas
 *
 * @returns No devuelve nada
 */

void my_to_upper(std::string &word)
{
    transform(word.begin(), word.end(), word.begin(), [](unsigned char c)
              { return toupper(c); });
}

/**
 * Convierte una palabra a minúsculas.
 *
 * @param word Palabra que se quiere convertir a minúsculas
 *
 * @returns No devuelve nada
 */

void my_to_lower(std::string &word)
{
    transform(word.begin(), word.end(), word.begin(), [](unsigned char c)
              { return tolower(c); });
}