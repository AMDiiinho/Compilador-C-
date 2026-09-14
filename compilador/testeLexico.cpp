#include "LexicoInterface.hpp"
#include <iostream>

int main() {
    Lexico lexico("contexto/correto.cpp.txt");

    while (true) {
        Token token = lexico.proximoToken();

        std::cout
            << "tipo: " << static_cast<int>(token.tipo)
            << " | lexema: " << token.lexema
            << " | linha: " << token.linha
            << " | coluna: " << token.coluna
            << '\n';

        if (token.tipo == TipoToken::FIMARQUIVO) {
            break;
        }
    }
}