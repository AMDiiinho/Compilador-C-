#include "LexicoInterface.hpp"
#include "TokenInterface.hpp"
#include <stdexcept>
#include <fstream>
#include <iterator>
#include <cctype>

//Implementando o construtor Lexico

Lexico::Lexico(const std::string& caminhoArquivo) {
    std::ifstream arquivo(caminhoArquivo);    //criando um objeto arquivo que abre caminhoArquivo para leitura

    if (!arquivo) {
        throw std::runtime_error("Não foi possível abrir: " + caminhoArquivo);
    }

    //copiando todos os caracteres de arquivo para código
    codigo.assign(
        std::istreambuf_iterator<char>(arquivo),  //inicio do conteúdo do arquivo
        std::istreambuf_iterator<char>()          //fim do arquivo
    );
};


//Implementando o método proximoToken da classe Lexico que deverá retornar um tipo Token
Token Lexico::proximoToken() {

    ignorarEspacos();

    //checando se estou no fim do arquivo
    if (posicao >= codigo.size()) {
        return {TipoToken::FIMARQUIVO, "", linha, coluna};
    }

    // checando se o caracter atual é alfabético ou _ , se for, pode ser que seja um identificador ou palavra reservada, nesse caso chamo IdOuPalavra
    if (std::isalpha(atual()) || atual() == '_') {
        
        return IdOuPalavra();
    }

    //Identificando #includes:
    /* Vejo se apartir da posição atual, 8 posições para frente formo exatamente "#include", se sim, construo o token, avanço 8 posições
    e retorno o token */
    if(codigo.compare(posicao, 8, "#include") == 0) {

        Token token {TipoToken::INCLUDE, "#include", linha, coluna};

        for (int i = 0; i < 8; i++) {
            avancar();
        }

        return token;
    }

    // -- Reconhecendo os demais tokens como ), (, !, ==, !, =.... ---

    if (codigo.compare(posicao, 2, "==") == 0) {
        
        Token token {TipoToken::IGUAL, "==", linha, coluna};
        avancar();
        avancar();
        return token;
    }

    if (codigo.compare(posicao, 2, "!=") == 0) {
        
        Token token {TipoToken::DIFERENTE, "!=", linha, coluna};
        avancar();
        avancar();
        return token;
    }

    if (codigo.compare(posicao, 2, ">=") == 0) {
        
        Token token {TipoToken::MAIORIGUAL, ">=", linha, coluna};
        avancar();
        avancar();
        return token;
    }

    if (codigo.compare(posicao, 2, "<=") == 0) {
        
        Token token {TipoToken::MENORIGUAL, "<=", linha, coluna};
        avancar();
        avancar();
        return token;
    }

    if (codigo.compare(posicao, 2, "<<") == 0) {
        
        Token token {TipoToken::DESLOCAESQUERDA, "<<", linha, coluna};
        avancar();
        avancar();
        return token;
    }

    char caractere = atual();

    switch (caractere)
    {
    case '=':
        avancar();
        return {TipoToken::ATRIBUICAO, "=", linha, coluna - 1};
        break;

    case '+':
        avancar();
        return {TipoToken::SOMA, "+", linha, coluna - 1};
        break;

    case '-':
        avancar();
        return {TipoToken::SUBTRACAO, "-", linha, coluna - 1};
        break;

    case '*':
        avancar();
        return {TipoToken::MULTIPLICACAO, "*", linha, coluna - 1};
        break;

    case '/':
        avancar();
        return {TipoToken::DIVISAO, "/", linha, coluna - 1};
        break;

    case '>':
        avancar();
        return {TipoToken::MAIOR, ">", linha, coluna - 1};
        break;

    case '<':
        avancar();
        return {TipoToken::MENOR, "<", linha, coluna - 1};
        break;

    case '(':
        avancar();
        return {TipoToken::ABREPARENTESES, "(", linha, coluna - 1};
        break;


    case ')':
        avancar();
        return {TipoToken::FECHAPARENTESES, ")", linha, coluna - 1};
        break;

    case '{':
        avancar();
        return {TipoToken::ABRECHAVES, "{", linha, coluna - 1};
        break;

    case '}':
        avancar();
        return {TipoToken::FECHACHAVES, "}", linha, coluna - 1};
        break;

    case ',':
        avancar();
        return {TipoToken::VIRGULA, ",", linha, coluna - 1};
        break;

    case ';':
        avancar();
        return {TipoToken::PONTOEVIRGULA, ";", linha, coluna - 1};
        break;
    
    }

    if (std::isdigit(atual())) {
        return numeroReal();
    }

    throw std::runtime_error("Caractere inválido");
  
};

void Lexico::ignorarEspacos() {

    //Equanto minha posição não passou do fim do código
    while (posicao < codigo.size()) {
        //procuro por espaços em branco, se o atual() for, sigo avançando a leitura
        if (atual() == ' ' || atual() == '\t' || atual() == '\r') {
            
            avancar();
        //caso eu encontre uma quebra de linha
        } else if (atual() == '\n') { 
            
            avancar();      //avanço 1 caracter
            linha++;        //atualizo a linha
            coluna = 1;     //reinicio a coluna
        } else {

            break;
        }
        
    }
};


//implementando avancar()
char Lexico::avancar() {

    char caractere = codigo[posicao]; //o caracter torna-se o proximo caracter em codigo[posicao]
    posicao++;                        //atualizando posição
    coluna++;                         //atualizando coluna
    return caractere;                 //retornando o caracter lido
};

//implementando atual(), não deve alterar os atributos de Lexico
char Lexico::atual() const {
    
    return codigo[posicao];           //retorna o caracter atual
}


Token Lexico::IdOuPalavra() {

    int linhaInicial = linha;
    int colunaInicial = coluna;
    std::string lexema;

    //enquanto eu estiver lendo um caracter alfanumérico ou _
    while (posicao < codigo.size() && (std::isalnum(atual()) || atual() == '_')) {  

        //concateno o caracter atual() para formar o lexema
        lexema += atual();
        //avanço a leitura
        avancar();
    }

    if (lexema == "int") {
        return {TipoToken::INT, lexema, linhaInicial, colunaInicial};
    };
    
    if (lexema == "double") {
        return {TipoToken::DOUBLE, lexema, linhaInicial, colunaInicial};
    };

    if (lexema == "if") {
        return {TipoToken::IF, lexema, linhaInicial, colunaInicial};
    };

    if (lexema == "iostream") {
        return {TipoToken::IOSTREAM, lexema, linhaInicial, colunaInicial};
    };

    if (lexema == "lerDouble") {
        return {TipoToken::LERDOUBLE, lexema, linhaInicial, colunaInicial};
    };
    
    if (lexema == "else") {
        return {TipoToken::ELSE, lexema, linhaInicial, colunaInicial};
    };

    if (lexema == "while") {
        return {TipoToken::WHILE, lexema, linhaInicial, colunaInicial};
    };

    if (lexema == "cout") {
        return {TipoToken::COUT, lexema, linhaInicial, colunaInicial};
    };

    if (lexema == "main") {
        return {TipoToken::MAIN, lexema, linhaInicial, colunaInicial};
    };

    if (lexema == "std") {
        return {TipoToken::STD, lexema, linhaInicial, colunaInicial};
    };

    if (lexema == "endl") {
        return {TipoToken::ENDL, lexema, linhaInicial, colunaInicial};
    };

    if (lexema == "using") {
        return {TipoToken::USING, lexema, linhaInicial, colunaInicial};
    };

    if (lexema == "namespace") {
        return {TipoToken::NAMESPACE, lexema, linhaInicial, colunaInicial};
    };

    return {TipoToken::IDENTIFICADOR, lexema, linhaInicial, colunaInicial};
}

Token Lexico::numeroReal() {

    int linhaInicial = linha;
    int colunaInicial = coluna;
    std::string lexema;

    //lendo o caractere numérico do número real antes do ponto "."
    while (posicao < codigo.size() && std::isdigit(atual())) {

        //concatenando o caracter numérico lido para formar o lexema
        lexema += atual();
        avancar();
    }

    //identificando o "."
    if (posicao < codigo.size() && atual() == '.') {
        lexema += atual();
        avancar();

        //lendo o restante do número após o ponto "."
        while (posicao < codigo.size() && std::isdigit(atual())) {
            lexema += atual();
            avancar();
        }
    }

    return {TipoToken::NUMEROREAL, lexema, linhaInicial, colunaInicial};
};