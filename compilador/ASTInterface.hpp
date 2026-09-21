#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <iosfwd>


// Representa o tipo de operador armazenado em um nó. Menos é um valor do tipo OperadorUnario usado por ExpressaoUnaria
enum class OperadorUnario {
  
    Menos
};

// Representa os tipos de operadores binários. Subtracao é um valor do tipo OperadorBinario usado por ExpressaoBinaria
enum class OperadorBinario {

    Soma,
    Subtracao,
    Multiplicacao,
    Divisao
};

// Declarando a classe base de todas as outras expressoes. Virtual permite a identificacao do tipo real do objeto ao destrui-lo
// através de unique_ptr<Expressao>
struct Expressao {
    
    //~Expressao() é um destruidor, e default gera sua implementação padrão
    //~Expressao() é chamado quando um objeto deixa de existir
    //o virtual garante que quando um objeto derivado de expressao for destruido através de um ponteiro para expressao, o destrutor da classe
    //também será executado,
    //default cria a implementaçaõ padrão
    virtual ~Expressao() = default;
};

// Ponteiro para Expressao, cria posses de nó. Ex: ExpressaoBinaria é dona dos nós esquerda e direita, que são os operandos
// using declara um novo nome para um tipo já existente
// ExpressaoPtr é um nome mais curto para std::unique_ptr<Expressao>
using ExpressaoPtr = std::unique_ptr<Expressao>;

// Declaração de Identificador, que herda de Expressão, ":" aqui é o equivalente ao "extends" do java
struct Identificador : Expressao {

    std::string id;
    // Nó que representa o uso de um id em uma Expressao, como "a + 2"
    // Explicit impede uma possível conversão de uma std::string em um id
    // Identificador recebe uma string id como pametro
    // inicializando os atributos do objeto, inicializo atributo id com o parametro id, transferindo seu valor
    explicit Identificador(std::string id) : id(std::move(id)) {}

    
};

// isso identifica um valor numérico mas tenho as mesmas dúvidas do de cima
struct Numero : Expressao {

    std::string valor;

    //Nó que representa o uso de um número em uma expressão como "2 + 2"
    explicit Numero(std::string valor) : valor(std::move(valor)) {}

   
};

// Recebe uma entrada "lerDouble"
struct Leitura : Expressao {};

// declaração da estrutura da expressão unaria, ela exige um operador e um operando como parametro
// operador(operador) inicializa o atributo operador com o valor recebido por parâmetro
// operando(std::move(operando)) transfere a posse do nó da expressão do parametro para o atriburo
// e ela possui operador do tipo OperadorUnario e operando do tipo ExpressaoPtr
struct ExpressaoUnaria : Expressao {

    OperadorUnario operador;
    ExpressaoPtr operando;

    ExpressaoUnaria(OperadorUnario operador, ExpressaoPtr operando) 
        : operador(operador), operando(std::move(operando)) {}

};


// declaracao da estrutura da expressao binaria
struct ExpressaoBinaria : Expressao {

    // o operador realizará a operação com a expressão esquerda e a da direita, que podem ser apenas um número, ou uma expressao mais complexa
    ExpressaoBinaria (ExpressaoPtr esquerda, OperadorBinario operador, ExpressaoPtr direita)
        : esquerda(std::move(esquerda)), operador(operador), direita(std::move(direita)) {}
    
    ExpressaoPtr esquerda;
    OperadorBinario operador;
    ExpressaoPtr direita;
}; 

// Declaração dos tipos de operação relacionais que podem ser armazenados em nós
enum class OperadorRelacional {

    Igual,
    Diferente,
    Maior,
    Menor,
    MaiorIgual,
    MenorIgual
};

// Declaracao da extrutura de uma condicao if (esquerda operador direita)
struct Condicao {

    ExpressaoPtr esquerda;
    OperadorRelacional operador;
    ExpressaoPtr direita;
};

// Classe base para todos os comandos do programa
struct Comando {

    virtual ~Comando() = default;
};

// Ponteiro com posse de um comando
using ComandoPtr = std::unique_ptr<Comando>;

// Representando uma declaração, como, double a, b, c;
// Declaracao herda de comando
struct Declaracao : Comando {

    std::vector<std::string> ids;

    //Construtor de Declaracao
    //std::move é uma função da biblioteca padrão
    //define que o parâmetro ids pode ter seus recursos transferidos para outro objeto
    //o vetor membro recebe os dados do vetor parâmetro

    // o equivalente a esse construtor em java seria

    /*
        Declaracao(tipo ids) {

            this.ids = ids;
        }
    
    */
    explicit Declaracao(std::vector<std::string> ids) : ids(std::move(ids)) {}
};

// Representando uma atribuição
struct Atribuicao : Comando {

    std::string destino;
    ExpressaoPtr expressao;

    /*
        Atribuicao(string destino, ExpressaoPointer expressao) {
        
            this.destino = destino;
            this.expressao = expressao;
        }
    */
    
    Atribuicao(std::string destino, ExpressaoPtr expressao) : destino(std::move(destino)), expressao(std::move(expressao)) {}
};


// Representando um print
struct Print : Comando {
    
    std::string id;

    explicit Print(std::string id) : id(std::move(id)) {}
};

// Representando um If
struct Se : Comando { 
    
    Condicao condicao;
    //vetor de comandos
    std::vector<ComandoPtr> entao;
    std::vector<ComandoPtr> senao;
};

// Representando um while
struct Enquanto : Comando {

    Condicao condicao;
    std::vector<ComandoPtr> corpo;
};

// Nó raíz da arvore

struct Programa {
    
    std::vector<ComandoPtr> comandos;
};

//ostream é um tipo que representa uma saída de texto
// será usado depois para imprimir a AST
void imprimirAST(const Programa& programa, std::ostream& saida);