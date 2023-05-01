#include <iostream>
#include <thread>
#include <string>
#include <queue>
// #include <req_struct.h>
#include <thread_struct.h>
#include <mutex>
#include <atomic>
#include <fstream>
// #include <my_lib.h>

#define TH_NUM 4

class Searcher
{
protected:
    int id;
    std::string word;

public:
    Searcher(int id, std::string word)
    {
        this->word = word;
        this->id = id;
    }

    int getId() const
    {
        return id;
    }

    std::string getWord() const
    {
        return word;
    }
};

class FreeSearcher : public Searcher
{
public:
    int words_num;
    std::atomic<int> word_counter;
    // struct thread_struct results;
    std::vector<thread_struct> v_thread_struct;
    std::shared_mutex searcher_mtx;
    std::vector<std::thread> v_threads_books;

public:
    FreeSearcher(int id, std::string word, int words_num) : Searcher(id, word)
    {
        this->words_num = words_num;
        this->word_counter = 0;
    }

    int getWordsNum() const
    {
        return words_num;
    }

    int getId() const
    {
        return id;
    }

    void new_search(std::string file_name, std::string word)
    {

        //std::cout << "Searching in book: " << file_name << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(4000));
        int lines_num = count_lines_in_file(file_name);
        int lines_per_thread = lines_num / TH_NUM;
        for (int i = 0; i < TH_NUM; i++)
        {
            // Se dividen las líneas entre los hilos
            int id = i;
            int init_line = i * lines_per_thread;
            int final_line = init_line + lines_per_thread - 1;
            if (i == TH_NUM - 1)
            {
                final_line = lines_num - 1;
            }

            v_thread_struct.push_back(thread_struct{id, init_line, final_line, word, std::queue<result_struct>()});
            // Se crean los hilos y se añaden en el vector de hilos, a su vez se llama a la función find_word_in_file,
            // que es la que se encarga de buscar la palabra en el fichero.
            // v_threads_books.push_back(std::thread(find_word_in_file, file_name, i));
            v_threads_books.push_back(std::thread(&FreeSearcher::find_word_in_file, this, file_name, i));
        }

        std::for_each(v_threads_books.begin(), v_threads_books.end(), std::mem_fn(&std::thread::join));
        
    }

    void find_word_in_file(std::string file_name, int thread_v_pos)
    {
        /*  Se inician y se crean las variables necesarias para la función. */
        int current_line = 0;
        std::fstream file;
        std::string line;
        // std::cout << words_num << std::endl;
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
                while (words_in_line >> current_word && word_counter <= words_num)
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
                        std::unique_lock<std::shared_mutex> lock(searcher_mtx);
                        
                        v_thread_struct[thread_v_pos].result.push(result_str);
                        lock.unlock();

                        word_counter++;
                        //std::cout << "Word counter: " << word_counter << std::endl;
                    }
                    previous_word = current_word;
                }
            }
            current_line++;
        }
        // Se cierra el fichero.
        file.close();
    }
};