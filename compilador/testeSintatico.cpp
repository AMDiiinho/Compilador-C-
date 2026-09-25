#include "SintaticoInterface.hpp"
#include "GeradorCodigoInterface.hpp"
#include <iostream>
#include "SemanticoInterface.hpp"
#include <fstream>

int main(int argc, char* argv[]) {
    try {
        const std::string caminho = argc > 1
            ? argv[1]
            : "contexto/correto.cpp.txt";

        Lexico lexico(caminho);
        Sintatico sintatico(lexico);
        Semantico semantico;

        Programa programa = sintatico.analisar();

        TabelaSimbolos tabela = semantico.analisar(programa);

        GeradorCodigo gerador;
        const std::vector<std::string> codigo = gerador.gerar(programa, tabela);
        const std::string caminhoObjeto = argc > 2 ? argv[2] : "programa.obj";

        std::ofstream arquivoObjeto(caminhoObjeto);

        if (!arquivoObjeto) {

            throw std::runtime_error("Não foi possível criar o arquivo objeto.");
        };

        for (const std::string& instrucao : codigo) {

            arquivoObjeto << instrucao << '\n';
        }

        std::cout << "Arquivo gerado em: " << caminhoObjeto << '\n';

        std::cout << "Programa semanticamente valido.\n";
        std::cout << "Codigo objeto:\n";
        for (const std::string& instrucao : codigo) {
            std::cout << instrucao << '\n';
        }

        std::cout << "Programa valido.\n";
        imprimirAST(programa, std::cout);
    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << '\n';
        return 1;
    }
}
