#pragma once

#include "ASTInterface.hpp"
#include "TabelaSimbolos.hpp"

#include <string>
#include <vector>



class GeradorCodigo {

    public:
        // Gera o programa completo para a MaqHipo, incluindo inicialização e encerramento.
        std::vector<std::string> gerar(const Programa& programa, const TabelaSimbolos& tabela) const;
    
    private:
        // Gera os comandos de um bloco na ordem em que aparecem na AST.
        static void gerarComandos(const std::vector<ComandoPtr>& comandos, const TabelaSimbolos& tabela, std::vector<std::string>& codigo);

        // Converte um comando da AST para instruções da MaqHipo.
        static void gerarComando(const Comando& comando, const TabelaSimbolos& tabela, std::vector<std::string>& codigo);

        // Converte uma expressão e deixa o resultado no topo da pilha de dados.
        static void gerarExpressao(const Expressao& expressao, const TabelaSimbolos& tabela, std::vector<std::string>& codigo);

        // Converte uma condição em instruções de comparação.
        static void gerarCondicao(const Condicao& condicao, const TabelaSimbolos& tabela, std::vector<std::string>& codigo);

        // Atualiza o endereço de destino de um salto criado anteriormente.
        static void gerarSalto(std::vector<std::string>& codigo, std::size_t indice, std::size_t destino);
};
