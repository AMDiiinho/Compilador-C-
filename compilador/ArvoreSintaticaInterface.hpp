#pragma once

#include "TokenInterface.hpp"

#include <memory>
#include <optional>
#include <variant>
#include <vector>

// Não terminais
enum class NaoTerminal {
    PROG, DC, VAR, VARS, MAIS_VAR, TIPO, CMDS, MAIS_CMDS,
    CMD_COND, CMD, PFALSA, RESTO_IDENT, EXP_IDENT, CONDICAO,
    RELACAO, EXPRESSAO, TERMO, OP_UN, FATOR, OUTROS_TERMOS,
    OP_AD, MAIS_FATORES, OP_MUL
};

using Simbolo = std::variant<TipoToken, NaoTerminal>;   //Declarando Simbolo, pode ser um TipoToken ou NaoTerminal
                                                        //TipoToken::INCLUDE    //terminal
                                                        //NaoTerminal::PROG     //não terminal

// Nó da árvore concreta
// Preserva todos os simbolos usados pela gramática

// um nó possui um simbolo, pode ter um token ou não, e possui um vetor de ponteiros para nó filhos
struct No {

    Simbolo simbolo;
    std::optional<Token> token;
    std::vector<std::unique_ptr<No>> filhos;

    //construtor de simbolo; this.simbolo = simbolo
    explicit No(Simbolo simbolo) : simbolo(simbolo) {}
};