#include "PilhaDadosInterface.hpp"
#include <cstddef>
#include <stdexcept>


void PilhaDados::empilhar(double valor) {

    dados.push_back(valor);
};

double PilhaDados::desempilhar() {

    if (dados.empty()) {

        throw std::runtime_error("Houve uma tentativa de desempilhar, porém a pilha vazia!");
    } else {

        //armazenando o dado em valor, .back() pega o valor do topo da pilha
        double valor = dados.back();

        //removendo o dado da pilha
        dados.pop_back();

        //retornando o valor
        return valor;
    };
};

double& PilhaDados::topo() {

    if (dados.empty()) {

        throw std::runtime_error("Houve uma tentativa de pegar o valor do topo da pilha, porém a pilha vazia!");     
    };
    
    //retornando o valor do topo da pilha
    return dados.back();
};

double& PilhaDados::antecessor() {

    if (dados.size() < 2) {
        throw std::runtime_error("Houve uma tentativa de pegar o valor do antecessor da pilha, porém, não há dados suficientes na pilha!");
    };

    //retornando o valor do dado logo abaixo do topo
    return dados[dados.size() - 2];
};

void PilhaDados::limpar() {
    
    dados.clear();
};


double PilhaDados::ler(std::size_t endereco) const {
    //retornando o valor do dado em um endereço específico
    return dados.at(endereco);
};

void PilhaDados::armazenar(std::size_t endereco, double valor) {
    //armazenando um valor em um endereço específico
    dados.at(endereco) = valor;
};

void PilhaDados::alocar(std::size_t quantidade) {
    //alocando um valor x de endereços na pilha
    dados.insert(dados.end(), quantidade, 0.0);
};