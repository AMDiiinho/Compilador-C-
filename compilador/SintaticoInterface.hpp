#pragma once
#include "LexicoInterface.hpp"

class Sintatico {


    public:

        //Declarando o método construtor sintático que receberá uma referência ao objeto Lexico e pedirá os tokens usando lexico.proximoToken()
        //(o explicit impede que alterações de tipo sejam realizadas implicitamente)
        explicit Sintatico(Lexico& lexico);

        //declarando o método analisar()
        void analisar();
    
    private:

        //guardndo uma referência ao Lexico
        Lexico& lexico;
};