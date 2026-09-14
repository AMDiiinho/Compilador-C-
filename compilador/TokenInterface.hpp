#pragma once
#include <string>

enum class TipoToken {

    INCLUDE, IOSTREAM, USING, NAMESPACE, STD,
    INT, MAIN, DOUBLE, IF, ELSE, WHILE,
    COUT, ENDL, LERDOUBLE, IDENTIFICADOR,
    NUMEROREAL, ATRIBUICAO, SOMA, SUBTRACAO, MULTIPLICACAO,
    DIVISAO, IGUAL, DIFERENTE, MAIOR, MENOR, MAIORIGUAL,
    MENORIGUAL, DESLOCAESQUERDA, ABREPARENTESES,
    FECHAPARENTESES, ABRECHAVES, FECHACHAVES,
    VIRGULA, PONTOEVIRGULA, FIMARQUIVO

};

struct Token {

    TipoToken tipo;      //o tipo do token, sendo DESLOCAAESQUERDA, ABREPARENTESES, SOMA.....
    std::string lexema;  //o trecho literal lido, de fato, como double, if, while, else
    int linha;           //linha do token
    int coluna;          //coluna do token
};

