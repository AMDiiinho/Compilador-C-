#include "SintaticoInterface.hpp"
#include <iostream>

int main() {
    try {
        Lexico lexico("contexto/minimo.cpp.txt");
        Sintatico sintatico(lexico);

        sintatico.analisar();

        std::cout << "Programa valido.\n";
    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << '\n';
    }
}