#pragma once
#include "ASTInterface.hpp"
#include "LexicoInterface.hpp"
#include <memory>

struct No;

class Sintatico {


    public:

        //Declarando o método construtor sintático que receberá uma referência ao objeto Lexico e pedirá os tokens usando lexico.proximoToken()
        //(o explicit impede que alterações de tipo sejam realizadas implicitamente)
        explicit Sintatico(Lexico& lexico);
        ~Sintatico();

        //declarando o método analisar() ------  ANTES DA AST -------
        //void analisar();

        //declarando o método analisar() ------ COM A AST -------
        //analisar vê se o programa é válido e agora retorna um tipo Programa, que é um vetor de comandos
        Programa analisar();
        const No& arvoreSintatica() const;
    
    private:

        //guardndo uma referência ao Lexico
        Lexico& lexico;
        std::unique_ptr<No> raiz;
};
