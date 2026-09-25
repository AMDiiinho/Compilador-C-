#pragma once

#include <string>
#include <unordered_map>

struct Simbolo {

    int endereco;
};

class TabelaSimbolos {
public:

    bool declarar(const std::string& nome);
    bool contem(const std::string& nome) const;
    
    // const Simbolo&: devolve uma referência para o símbolo armazenado, porém, quem recebe, não pode modificá-lo
    // const std::string& nome: recebe o nome por referência e garante que o método não vai alterá-lo
    // const final: garante que o método obter não altera a própria TabelaSimbolos, assim, pode ser chamado mesmo sobre uma tabela constante
    const Simbolo& obter(const std::string& nome) const;
    int quantidade() const;

private:

    std::unordered_map<std::string, Simbolo> simbolos;
    int proximoEndereco = 0;
};
