#include <iostream>

// Estructura para las peticiones de búsqueda con toda la información del cliente. 
struct sh_request
{
    int client_id;
    int client_type;
    int balance;
    std::string word;
};