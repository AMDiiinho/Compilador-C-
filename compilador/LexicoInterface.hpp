#pragma once

#include "TokenInterface.hpp"
#include <string>

class Lexico {
    
    public:
        Lexico(const std::string& caminhoArquivo);  //o analisador léxico de fato, construido apartir do arquivo-fonte
        Token proximoToken();                       //retorna o próximo token
        

    private:
        std::string codigo;         //arquivo-fonte
        std::size_t posicao = 0;    //índice do caracter atual
        int linha = 1;              //qual linha está o caracter?
        int coluna = 1;             //qual coluna está o caracter?
        char atual() const;         //retorna o caracter atual
        char avancar();             //passa para o próximo caracter do arquivo
        void ignorarEspacos();      //se o caracter atual for um espaço, avança novamente
        Token IdOuPalavra();        //le uma sequencia de caracteres, depois decide se é palavra reservada ou IDENTIFICADOR
        Token numeroReal();
};
