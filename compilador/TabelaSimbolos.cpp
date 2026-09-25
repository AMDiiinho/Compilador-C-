#include "TabelaSimbolos.hpp"

// recebe uma string nome e tenta criar um registro no mapa contendo esse nome e o endereço, caso ele consiga, retorna 1, se não 0
// Ex: declaração double a, b
// {a, 0}, {b, 1}...
bool TabelaSimbolos::declarar(const std::string& nome) {
    const auto resultado = simbolos.emplace(nome, Simbolo{proximoEndereco});

    //checando se eu consegui criar um registro no mapa
    if (resultado.second) {
        ++proximoEndereco;
    }

    //retornando o resultado da tentativa
    return resultado.second;
}

// recebe uma string nome e retorna se esse nome existe na tabela ou não
bool TabelaSimbolos::contem(const std::string& nome) const {
    
    return simbolos.find(nome) != simbolos.end();
}

// retorna o simbolo do nome passado
const Simbolo& TabelaSimbolos::obter(const std::string& nome) const {
    
    return simbolos.at(nome);
}

// retorna a quantidade de simbolos na tabela
int TabelaSimbolos::quantidade() const {
    
    return proximoEndereco;
}
