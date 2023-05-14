#include <iostream>
#include <thread>

#define MAX_CLIENTS 5
#define MAX_SEARCHERS 4
#define TH_NUM std::thread::hardware_concurrency()
#define WORDS_NUM 5
#define RESET   "\033[0m"
#define RED     "\033[91;1m"
#define GREEN   "\033[32;1m"
#define YELLOW  "\033[33;1m"
#define BLUE    "\033[34;1m"
#define MAGENTA "\033[35;1m"
#define CYAN    "\033[36;1m"
#define WHITE   "\033[37;1m"
#define LINE   "-------------------------------------------------------------------------------------"
#define WELCOME "WELCOME TO THE SEARCH PROGRAM"
