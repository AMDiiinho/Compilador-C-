#pragma once
#include <vector>
#include <cstddef>
#include <stdexcept>

class PilhaDados {

    public:

        // -- Operações de Pilha --

        //empilhando valores
        void empilhar(double valor);

        //desempilhando valores
        double desempilhar();

        //retornando uma referência ao valor dentro da pilha, e não uma cópia
        double& topo();
        double& antecessor();

        //esvaziando a pilha
        void limpar();

        // -- Operações de memória --

        //Ler um valor na pilha
        double ler(std::size_t endereco) const;

        //armazenar um valor na pilha
        void armazenar(std::size_t endereco, double valor);

        //alocar uma quantia de endereços
        void alocar(std::size_t quantidade);

    private:

        std::vector<double> dados;
};
