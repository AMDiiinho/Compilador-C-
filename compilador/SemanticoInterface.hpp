#pragma once

#include "ASTInterface.hpp"
#include "TabelaSimbolos.hpp"

#include <stdexcept>
#include <string>
#include <vector>


class ErroSemantico : public std::runtime_error {
public:
    explicit ErroSemantico(const std::string& mensagem);
};

class Semantico {
public:
    TabelaSimbolos analisar(const Programa& programa) const;

private:
    static void analisarComandos(const std::vector<ComandoPtr>& comandos, TabelaSimbolos& tabela);
    static void analisarComando(const Comando& comando, TabelaSimbolos& tabela);
    static void analisarCondicao(const Condicao& condicao, TabelaSimbolos& tabela);
    static void analisarExpressao(const Expressao& expressao, TabelaSimbolos& tabela);
    static void exigirDeclarada(const std::string& id, const TabelaSimbolos& tabela);
};
