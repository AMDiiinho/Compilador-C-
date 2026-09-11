//impede que o arquivo .hpp seja incluído mais de uma vez durante a compilação, caso outro arquivo inclua este,
//a classe poderia ser declarada duas vezes.
#pragma once
#include "InstrucaoInterface.hpp"
#include "PilhaDadosInterface.hpp"
#include <string>
#include <vector>

// MaqHipo

// Guarda o programa, a memória e o estado atual da execução
class Maquina {
    
    public:
        void carregarPrograma(const std::string& caminho);  // Abre o .obj e preenche o vetor de instruções C, recebe o caminho
        void executar();                                    // Interpreta as instruções em C e para com uma instrução de PARA

    private:
        std::vector<Instrucao> C; // código carregado (vetor de instruções de máquina)
        PilhaDados pilhaD;    // memória/pilha de dados (vetor que empilha os dados e desempilha ao realizar as operações)

        int i = 0;                // próxima instrução
        bool executando = false;  // permanece true durante executar() e falte após um PARA
  };
