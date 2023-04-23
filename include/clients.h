#include <iostream>
#include <thread>
#include <string>
#include <queue>


class Cliente {
    protected:
        int id;

    public:
        Cliente(int id) {
            this->id = id;
        }

        int getId() const {
            return id;
        }
};

class ClienteGratuito : public Cliente {
    public:
        ClienteGratuito(int id) : Cliente(id) {
            // Constructor por defecto
        }
};

class ClientePremium : public Cliente {
    private:
        double saldo;

    public:
        ClientePremium(int id, double saldo) : Cliente(id) {
            this->saldo = saldo;
        }

        double getSaldo() const {
            return saldo;
        }

        void setSaldo(double saldo) {
            this->saldo = saldo;
        }
};

class ClientePremiumIlimitado : public ClientePremium {
    public:
        ClientePremiumIlimitado(int id) : ClientePremium(id, 0) {
            // Constructor por defecto
        }
};
