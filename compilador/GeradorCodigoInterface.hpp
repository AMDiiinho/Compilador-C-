#pragma once

#include "ASTInterface.hpp"
#include "TabelaSimbolos.hpp"

#include <string>
#include <vector>



class GeradorCodigo {

    public:
        std::vector<std::string> gerar(const Programa& programa, const TabelaSimbolos& tabela) const;
    
    private:
        static void gerarComandos(const std::vector<ComandoPtr>& comandos, const TabelaSimbolos& tabela, std::vector<std::string>& codigo);
        static void gerarComando(const Comando& comando, const TabelaSimbolos& tabela, std::vector<std::string>& codigo);
        static void gerarExpressao(const Expressao& expressao, const TabelaSimbolos& tabela, std::vector<std::string>& codigo);
        static void gerarCondicao(const Condicao& condicao, const TabelaSimbolos& tabela, std::vector<std::string>& codigo);
        static void gerarSalto(std::vector<std::string>& codigo, std::size_t indice, std::size_t destino);
};