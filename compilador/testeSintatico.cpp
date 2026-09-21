#include "SintaticoInterface.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        const std::string caminho = argc > 1
            ? argv[1]
            : "contexto/correto.cpp.txt";

        Lexico lexico(caminho);
        Sintatico sintatico(lexico);

        Programa programa = sintatico.analisar();

        std::cout << "Programa valido.\n";
        imprimirAST(programa, std::cout);
    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << '\n';
        return 1;
    }
}
