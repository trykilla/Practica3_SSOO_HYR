#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cliente.h> // Incluir la definición de las clases de cliente

int main() {
    srand(time(NULL));
    int random = rand() % 3 + 1; // Cambiar el rango para incluir el valor 2

    switch (random) {
        case 1:
            std::cout << "Se va a generar un cliente gratuito" << std::endl;
            ClienteGratuito clienteGratuito(1);
            break;
        case 2:
            std::cout << "Se va a generar un cliente premium" << std::endl;
            ClientePremium clientePremium(1, 100);
            break;
        case 3:
            std::cout << "Se va a generar un cliente premium ilimitado" << std::endl;
            ClientePremiumIlimitado clientePremiumIlimitado(1);
            break;
    }

    return 0; // Agregar un valor de retorno explícito
}
