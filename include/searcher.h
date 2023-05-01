#include <iostream>
#include <thread>
#include <string>
#include <queue>
#include <req_struct.h>
#include <mutex>

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
