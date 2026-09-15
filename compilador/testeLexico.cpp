#include "LexicoInterface.hpp"
#include <iostream>

int main() {
    Lexico lexico("contexto/correto.cpp.txt");
    bool primeiro = true;

    std::cout << '{';

    while (true) {
        Token token = lexico.proximoToken();

        if (!primeiro) {
            std::cout << ", ";
        }

        std::cout << (token.tipo == TipoToken::FIMARQUIVO ? "$" : token.lexema);
        primeiro = false;

        if (token.tipo == TipoToken::FIMARQUIVO) {
            break;
        }
    }

    std::cout << "}\n";
}
