// executando de fato o arquivo objeto

#include "MaquinaInterface.hpp"

#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo.obj>\n";
        return 1;
    }

    try {
        // Instanciando uma máquina
        Maquina maquina;

        // Chamando a função carregar programa com o caminho informado no terminal
        maquina.carregarPrograma(argv[1]);

        //exibindo feedback de sucesso
        std::cout << "Programa carregado com sucesso. \n";

        maquina.executar();
    }
    catch (const std::exception& erro) {

        //imprimindo o erro em caso de falha
        std::cerr << "Erro: " << erro.what() << '\n';
        return 1;
    }

    return 0;
}
