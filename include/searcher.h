#include <iostream>
#include <thread>
#include <string>
#include <queue>
// #include <req_struct.h>
#include <thread_struct.h>
#include <mutex>
#include <atomic>
#include <fstream>
#include <shared_mutex>
#include <def.h>
// #include <my_lib.h>

void pay_system();

std::queue<std::shared_ptr<request>> payment_queue;
std::mutex pay_mtx;
std::condition_variable payment_cv;
std::mutex waiting_mtx;

class Searcher
{
protected:
    int id;
    int type;
    std::string word;

public:
    Searcher(int id, std::string word, int type)
    {
        this->word = word;
        this->id = id;
        this->type = type;
    }

    int getId() const
    {
        return id;
    }

    int getType() const
    {
        return type;
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
    std::atomic<int> credit_counter;
    std::vector<thread_struct> v_thread_struct;
    std::shared_mutex searcher_mtx;
    std::vector<std::thread> v_threads_books;

public:
    FreeSearcher(int id, std::string word, int type, int words_num) : Searcher(id, word, type)
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

    void new_search(std::string file_name, std::string word, std::vector<thread_struct> &v_thread_struct_client)
    {

        // std::cout << "Searching in book: " << file_name << std::endl;
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
            std::cout << "Book" << file_name << " - Thread " << i << " - Lines: " << init_line << " - " << final_line << std::endl;
            v_thread_struct.push_back(thread_struct{id, init_line, final_line, word, std::queue<result_struct>()});

            // Se crean los hilos y se añaden en el vector de hilos, a su vez se llama a la función find_word_in_file,
            // que es la que se encarga de buscar la palabra en el fichero.
            // v_threads_books.push_back(std::thread(find_word_in_file, file_name, i));

            v_threads_books.push_back(std::thread(&FreeSearcher::find_word_in_file, this, file_name, i));
        }

        std::for_each(v_threads_books.begin(), v_threads_books.end(), std::mem_fn(&std::thread::join));

        for (int i = 0; i < TH_NUM; i++)
        {
            std::cout << "from for:"
                      << "Thread " << i << " - Lines: " << v_thread_struct[i].initial_line << " - " << v_thread_struct[i].final_line << std::endl;
            v_thread_struct_client.push_back(v_thread_struct[i]);
        }

        v_thread_struct.clear();
        v_threads_books.clear();
    }

    void find_word_in_file(std::string file_name, int thread_v_pos)
    {

        /*  se inician y se crean las variables necesarias para la función. */
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
                while (words_in_line >> current_word && word_counter < words_num && credit_counter > 0)
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
                        if (this->getType() == 0)
                            word_counter++;
                        if (this->getType() == 1)
                            credit_counter--;
                        lock.unlock();

                        // std::cout << "Word counter: " << word_counter << std::endl;
                    }
                    previous_word = current_word;
                }
                waiting_mtx.lock();
                if (credit_counter == 0)
                {

                    srand(time(NULL));
                    std::shared_ptr<request> p = std::make_shared<request>();

                    p->balance = credit_counter;
                    p->mtx.lock();

                    std::unique_lock<std::mutex> lock(pay_mtx);
                    payment_queue.push(p);
                    lock.unlock();

                    payment_cv.notify_one();

                    p->mtx.lock();

                    credit_counter = p->balance;
                }
                waiting_mtx.unlock();
            }
            current_line++;
        }
        // Se cierra el fichero.
        file.close();
    }
};

void pay_system()
{

    std::cout << "Pay system started." << std::endl;

    std::unique_lock<std::mutex> lock(pay_mtx);
    payment_cv.wait(lock, []()
                    { return !payment_queue.empty(); });
    std::cout << "Paying..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    payment_queue.front()->balance = 100;
    payment_queue.front()->mtx.unlock();
    payment_queue.pop();
    std::cout << "Payment done." << std::endl;
}