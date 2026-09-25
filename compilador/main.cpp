#include "GeradorCodigoInterface.hpp"
#include "SemanticoInterface.hpp"
#include "SintaticoInterface.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <fonte.cpp.txt> <nome-programa.obj>\n";
        return 1;
    }

    try {
        Lexico lexico(argv[1]);
        Sintatico sintatico(lexico);
        Semantico semantico;
        GeradorCodigo gerador;

        Programa programa = sintatico.analisar();
        TabelaSimbolos tabela = semantico.analisar(programa);
        const std::vector<std::string> codigo = gerador.gerar(programa, tabela);

        const std::filesystem::path diretorioObjeto("codigo-objeto");
        std::filesystem::create_directories(diretorioObjeto);

        const std::filesystem::path caminhoObjeto = diretorioObjeto / argv[2];
        std::ofstream arquivoObjeto(caminhoObjeto);
        if (!arquivoObjeto) {
            throw std::runtime_error("Nao foi possivel criar o arquivo objeto.");
        }

        for (const std::string& instrucao : codigo) {
            arquivoObjeto << instrucao << '\n';
        }

        std::cout << "Compilacao concluida: " << caminhoObjeto.string() << '\n';
    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << '\n';
        return 1;
    }
}
