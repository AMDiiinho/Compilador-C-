/*
    Implementando a função carregarPrograma()

    Para cada linha do .obj:

    Ler a linha
    Retirar o nome da operação
    Tentar ler um arg
    Criar uma Instrucao
    Colocar a instrução no vetor C

    CRCT 10 --> {"CRCT", 10}
    SOMA    --> {"SOMA", 0}
*/

#include "MaquinaInterface.hpp"
#include "ConversorOperacaoInterface.hpp"
#include "PilhaDadosInterface.hpp"

#include <fstream>   // leitura de arquivos
#include <iostream>
#include <sstream>   // intrerpretação de texto da linha
#include <stdexcept> // exceptions

void Maquina::carregarPrograma(const std::string& caminho) {
    
    //abrindo e lendo o arquivo
    std::ifstream arquivo(caminho);

    // Resetando totalmente a máquina antes de iniciar uma nova execução:
    C.clear();
    i = 0;
    executando = false;
    std::string linha;

    while (std::getline(arquivo, linha)) {
        // criando o fluxo de leitura "leitor"
        std::istringstream leitor(linha);

        // lendo o nome em string da operacao
        std::string nomeOperacao;
        leitor >> nomeOperacao;

        // instanciando uma instrucao e convertendo a string lida no enum Operacao
        Instrucao instrucao;
        instrucao.operacao = converterOperacao(nomeOperacao);

        // lê um arg, se a instrução tiver um
        leitor >> instrucao.arg;

        // montando a lista de instruções na memória C para que o executar interprete posteriormente
        C.push_back(instrucao);
    }
}

void Maquina::executar() {

    //iniciando o estado de execução
    i = 0;
    executando = true;

    while (executando) {

        // Impedindo a possibilidade de acessar uma posição inexistente da lista de instruções C
        if (i < 0 || i >= static_cast<int>(C.size())) {
            throw std::runtime_error("Endereço de instrução inválido");
        }

        // Obtendo a instrução atual:
        const Instrucao& instrucao = C[i];
        i++;

        // implementando as operacoes
        switch (instrucao.operacao) {
           
        // Carrega a constante k (arg) no topo da pilha D
        case Operacao::CRCT:
            
            pilhaD.empilhar(instrucao.arg);
            break;

        // Carrega o valor de endereço n (arg) no topo da pilha D
        case Operacao::CRVL:
            
            pilhaD.empilhar(pilhaD.ler(instrucao.arg));
            break;

        /* Soma o elemento antecessor com o topo da pilha; desempilha os dois
        e empilha o resultado */
        case Operacao::SOMA: {

            double topo = pilhaD.desempilhar();
            double ant = pilhaD.desempilhar();
            pilhaD.empilhar(ant + topo);
            break;
        }

        // Subtrai o antecessor pelo elemento do topo
        case Operacao::SUBT: {
            
            double topo = pilhaD.desempilhar();
            double ant = pilhaD.desempilhar();
            pilhaD.empilhar(ant - topo);
            break;
        }

        // Multiplica o elemento antecessor pelo elemento do topo
        case Operacao::MULT: {

            double topo = pilhaD.desempilhar();
            double ant = pilhaD.desempilhar();
            pilhaD.empilhar(ant * topo);
            break;
        }

        //Divide o elemento antecessor pelo elemento do topo
        case Operacao::DIVI: {
            
            double topo = pilhaD.desempilhar();
            double ant = pilhaD.desempilhar();
            pilhaD.empilhar(ant / topo);
            break;
        }

        // Inverte o sinal do topo
        case Operacao::INVE:
            
            pilhaD.topo() = -pilhaD.topo();
            break;

        // Conjunção de valores lógicos. F = 0; V = 1
        case Operacao::CONJ: {
            
            double topo = pilhaD.desempilhar();
            double ant = pilhaD.desempilhar();
            pilhaD.empilhar((ant == 1 && topo == 1) ? 1 : 0);
            break;
        }

        //Disjunção de valores lógicos
        case Operacao::DISJ: {
            
            double topo = pilhaD.desempilhar();
            double ant = pilhaD.desempilhar();
            pilhaD.empilhar((ant == 1 || topo == 1) ? 1 : 0);
            break;
        }

        // Negação lógica
        case Operacao::NEGA:
            
            pilhaD.topo() = 1 - pilhaD.topo();
            break;

        // Comparação de menor entre o antecessor e o topo
        case Operacao::CPME: {
            
            double topo = pilhaD.desempilhar();
            double ant = pilhaD.desempilhar();
            pilhaD.empilhar((ant < topo) ? 1 : 0);
            break;
        }

        // Comparação de maior entre o antecessor e o topo
        case Operacao::CPMA: {
            
            double topo = pilhaD.desempilhar();
            double ant = pilhaD.desempilhar();
            pilhaD.empilhar((ant > topo) ? 1 : 0);
            break;
        }

        // Comparação de igualdade
        case Operacao::CPIG: {
            
            double topo = pilhaD.desempilhar();
            double ant = pilhaD.desempilhar();
            pilhaD.empilhar((ant == topo) ? 1 : 0);  
            break;
        }

        // Comparação de desigualdade
        case Operacao::CDES: {
            
            double topo = pilhaD.desempilhar();
            double ant = pilhaD.desempilhar();
            pilhaD.empilhar((ant != topo) ? 1 : 0);
            break;
        }

        // Comparação menor ou igual
        case Operacao::CPMI: {
            
            double topo = pilhaD.desempilhar();
            double ant = pilhaD.desempilhar();
            pilhaD.empilhar((ant <= topo) ? 1 : 0);
            break;
        }

        // Comparação maior ou igual
        case Operacao::CMAI: {
            
            double topo = pilhaD.desempilhar();
            double ant = pilhaD.desempilhar();
            pilhaD.empilhar((ant >= topo) ? 1 : 0);
            break;
        }

        // Armazena o topo da pilha no endereço n da pilha de dados (D)
        /*  ex:  a = a + 1
            
                CRVL a --> carrega o valor a
                CRCT 1 --> carrega a constante 1
                SOMA   --> desempilha os 2 valores, realiza a soma e empilha o resultado
                ARMZ a --> armazena em a
        */
        case Operacao::ARMZ: {
            
            double valor = pilhaD.desempilhar();
            pilhaD.armazenar(instrucao.arg, valor);
            break;
        };

        // desvio incondicional para a instrução de endereço p (arg)
        case Operacao::DSVI:
            i = instrucao.arg;
            break;

        /* desvio condicional para a instrução de endereço p (arg); 
        o desvio será executado caso a condição resultante seja falsa;
        o valor da condição estará no topo

            Ex:
            if x > 1 then
            x :=  z
            else
            y := 1
            CRVL x*
            CRCT 1
            CPMA
            DSVF ‘número instrução do else’
            CRVL z*
            ARMZ x*
            DSVI ‘número instrução depois do if’
            CRCT 1
            ARMZ y*
            */
        case Operacao::DSVF: {

            double condicao = pilhaD.desempilhar();

            if (condicao == 0) {
                i = instrucao.arg;
            }
            break;
        };

        // Lê um dado de entrada para o topo da pilha
        case Operacao::LEIT: {

            double valor;
            std::cin >> valor;
            
            if (!std::cin) { 
                throw std::runtime_error("Valor inválido!");
            }

            pilhaD.empilhar(valor);
            break;
        };

        // Imprime o valor do topo da pilha na saída
        case Operacao::IMPR:

            std::cout << pilhaD.desempilhar() << '\n';
            break;

        /* reserva m (arg) posições na pilha D; m depende do tipo
            da variável

            Ex
            var a, b: integer
            ALME 1
            ALME 1
            Ao mesmo tempo, na TS, coloca-se no campo de end_rel 
            de a e b, os valores s+1 e s+2 (0 e 1), respectivamente 
            (pois são as primeiras variáveis declaradas)
            */
        case Operacao::ALME:
            pilhaD.alocar(instrucao.arg);
            break;

        case Operacao::INPP:
            pilhaD.limpar();
            break;

        case Operacao::PARA:
            executando = false;
            break;
        }
    }
}
