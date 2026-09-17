#include "SintaticoInterface.hpp"
#include "LexicoInterface.hpp"
#include <variant>
#include <vector>
#include <utility>
#include <stdexcept>
#include <map>
#include <string>

enum class NaoTerminal {

    //Lista de não terminais existentes na gramática

    PROG, DC, VAR, VARS, MAIS_VAR, TIPO, CMDS, MAIS_CMDS,
    CMD_COND, CMD, PFALSA, RESTO_IDENT, EXP_IDENT, CONDICAO,
    RELACAO, EXPRESSAO, TERMO, OP_UN, FATOR, OUTROS_TERMOS,
    OP_AD, MAIS_FATORES, OP_MUL
};


using Simbolo = std::variant<TipoToken, NaoTerminal>;   //Declarando Simbolo, pode ser um TipoToken ou NaoTerminal
                                                        //TipoToken::INCLUDE    //terminal
                                                        //NaoTerminal::PROG     //não terminal

using Producao = std::vector<Simbolo>;                  //Declarando a producao que é um vetor de elementos do tipo Simbolo

using ChaveTabela = std::pair<NaoTerminal, TipoToken>;  //Declarando ChaveTabela, é um par (não terminal, token atual)
                                                        //{NaoTerminal::PROG, TipoToken::INCLUDE}, espero PROG, token é #include, qual a produção?        

using TabelaM = std::map<ChaveTabela, Producao>;        //Declarando a tabelaM que é um tipo map que possui uma ChaveTabela e uma Producao


std::string token(TipoToken tipo) {

    switch (tipo) {

        case TipoToken::INCLUDE: return "#include";
        case TipoToken::IOSTREAM: return "iostream";
        case TipoToken::USING: return "using";
        case TipoToken::NAMESPACE: return "namespace";
        case TipoToken::STD: return "std";
        case TipoToken::INT: return "int";
        case TipoToken::MAIN: return "main";
        case TipoToken::DOUBLE: return "double";
        case TipoToken::IF: return "if";
        case TipoToken::ELSE: return "else";
        case TipoToken::WHILE: return "while";
        case TipoToken::COUT: return "cout";
        case TipoToken::ENDL: return "endl";
        case TipoToken::LERDOUBLE: return "lerDouble";
        case TipoToken::IDENTIFICADOR: return "id";
        case TipoToken::NUMEROREAL: return "numero_real";
        case TipoToken::ATRIBUICAO: return "=";
        case TipoToken::SOMA: return "+";
        case TipoToken::SUBTRACAO: return "-";
        case TipoToken::MULTIPLICACAO: return "*";
        case TipoToken::DIVISAO: return "/";
        case TipoToken::IGUAL: return "==";
        case TipoToken::DIFERENTE: return "!=";
        case TipoToken::MAIOR: return ">";
        case TipoToken::MENOR: return "<";
        case TipoToken::MAIORIGUAL: return ">=";
        case TipoToken::MENORIGUAL: return "<=";
        case TipoToken::DESLOCAESQUERDA: return "<<";
        case TipoToken::ABREPARENTESES: return "()";
        case TipoToken::FECHAPARENTESES: return ")";
        case TipoToken::ABRECHAVES: return "{";
        case TipoToken::FECHACHAVES: return "}";
        case TipoToken::VIRGULA: return ",";
        case TipoToken::PONTOEVIRGULA: return ";";
        case TipoToken::FIMARQUIVO: return "$";
    }

    return "Token desconhecido!";
}

std::string descricaoToken(const Token& token) {
    if (token.tipo == TipoToken::FIMARQUIVO) {

        return "$";
    }

    return "'" + token.lexema + "'";
}


//Declarando o método que cria a tabelaM
TabelaM criarTabelaM() {

    //declarando tabela do tipo TabelaM
    TabelaM tabela;

    // -- DECLARAÇÃO DAS PRODUÇÕES -- //

    // O que estou vendo no topo da pilha? -> o que estou vendo na entrada?

    //se eu ver PROG no topo e #include na entrada, acesso a célula da tabela M que contém essas duas chaves, (M[PROG][INCLUDE])
    //desempilho prog e empilho a produção da regra
    // PROG -> #include
    tabela[{NaoTerminal::PROG, TipoToken::INCLUDE}] = {

        //Produção da regra
        TipoToken::INCLUDE,
        TipoToken::MENOR,
        TipoToken::IOSTREAM,
        TipoToken::MAIOR,
        TipoToken::USING,
        TipoToken::NAMESPACE,
        TipoToken::STD,
        TipoToken::PONTOEVIRGULA,
        TipoToken::INT,
        TipoToken::MAIN,
        TipoToken::ABREPARENTESES,
        TipoToken::FECHAPARENTESES,
        TipoToken::ABRECHAVES,
        NaoTerminal::CMDS,
        TipoToken::FECHACHAVES
    };

    //CMDS -> ''
    tabela[{NaoTerminal::CMDS, TipoToken::FECHACHAVES}] = { /*produz vazio*/ };

    //CMDS -> CMD MAIS_CMDS
    tabela[{NaoTerminal::CMDS, TipoToken::IDENTIFICADOR}] = {
        NaoTerminal::CMD,
        NaoTerminal::MAIS_CMDS
    };

    // CMDS -> CMD_COND CMDS
    tabela[{NaoTerminal::CMDS, TipoToken::IF}] = {
        NaoTerminal::CMD_COND,
        NaoTerminal::CMDS
    };

    // CMDS -> CMD_COND CMDS
    tabela[{NaoTerminal::CMDS, TipoToken::WHILE}] = {
        NaoTerminal::CMD_COND,
        NaoTerminal::CMDS
    };

    // CMD_COND -> while ( CONDICAO ) { CMDS }
    tabela[{NaoTerminal::CMD_COND, TipoToken::WHILE}] = {
        TipoToken::WHILE,
        TipoToken::ABREPARENTESES,
        NaoTerminal::CONDICAO,
        TipoToken::FECHAPARENTESES,
        TipoToken::ABRECHAVES,
        NaoTerminal::CMDS,
        TipoToken::FECHACHAVES
    };

    // CMD_COND -> if ( CONDICAO ) { CMDS } PFALSA
    tabela[{NaoTerminal::CMD_COND, TipoToken::IF}] = {
        TipoToken::IF,
        TipoToken::ABREPARENTESES,
        NaoTerminal::CONDICAO,
        TipoToken::FECHAPARENTESES,
        TipoToken::ABRECHAVES,
        NaoTerminal::CMDS,
        TipoToken::FECHACHAVES,
        NaoTerminal::PFALSA
    };

    // PFALSA -> else { CMDS }
    tabela[{NaoTerminal::PFALSA, TipoToken::ELSE}] = {
        TipoToken::ELSE,
        TipoToken::ABRECHAVES,
        NaoTerminal::CMDS,
        TipoToken::FECHACHAVES
    };

    // CONDICAO -> EXPRESSAO RELACAO EXPRESSAO
    tabela[{NaoTerminal::CONDICAO, TipoToken::IDENTIFICADOR}] = {
        NaoTerminal::EXPRESSAO,
        NaoTerminal::RELACAO,
        NaoTerminal::EXPRESSAO
    };

    tabela[{NaoTerminal::CONDICAO, TipoToken::NUMEROREAL}] = {
        NaoTerminal::EXPRESSAO,
        NaoTerminal::RELACAO,
        NaoTerminal::EXPRESSAO
    };

    tabela[{NaoTerminal::CONDICAO, TipoToken::ABREPARENTESES}] = {
        NaoTerminal::EXPRESSAO,
        NaoTerminal::RELACAO,
        NaoTerminal::EXPRESSAO
    };

    tabela[{NaoTerminal::CONDICAO, TipoToken::SUBTRACAO}] = {
        NaoTerminal::EXPRESSAO,
        NaoTerminal::RELACAO,
        NaoTerminal::EXPRESSAO
    };

    // RELACAO -> >
    tabela[{NaoTerminal::RELACAO, TipoToken::MAIOR}] = {
        TipoToken::MAIOR
    };

    // RELACAO -> ==
    tabela[{NaoTerminal::RELACAO, TipoToken::IGUAL}] = {
        TipoToken::IGUAL
    };

    //RELACAO -> !=
    tabela[{NaoTerminal::RELACAO, TipoToken::DIFERENTE}] = {
        TipoToken::DIFERENTE
    };

    //RELACAO -> >=
    tabela[{NaoTerminal::RELACAO, TipoToken::MAIORIGUAL}] = {
        TipoToken::MAIORIGUAL
    };

    //RELACAO -> <=
    tabela[{NaoTerminal::RELACAO, TipoToken::MENORIGUAL}] = {
        TipoToken::MENORIGUAL
    };

    //RELACAO -> <
    tabela[{NaoTerminal::RELACAO, TipoToken::MENOR}] = {
        TipoToken::MENOR
    };

    //MAIS FATORES -> ''
    tabela[{NaoTerminal::MAIS_FATORES, TipoToken::IGUAL}] = {};
    tabela[{NaoTerminal::MAIS_FATORES, TipoToken::DIFERENTE}] = {};
    tabela[{NaoTerminal::MAIS_FATORES, TipoToken::MAIORIGUAL}] = {};
    tabela[{NaoTerminal::MAIS_FATORES, TipoToken::MENORIGUAL}] = {};
    tabela[{NaoTerminal::MAIS_FATORES, TipoToken::MENOR}] = {};
    tabela[{NaoTerminal::MAIS_FATORES, TipoToken::MAIOR}] = {};
    tabela[{NaoTerminal::MAIS_FATORES, TipoToken::FECHAPARENTESES}] = {};

    //OUTROS_TERMOS -> ''
    tabela[{NaoTerminal::OUTROS_TERMOS, TipoToken::IGUAL}] = {};
    tabela[{NaoTerminal::OUTROS_TERMOS, TipoToken::DIFERENTE}] = {};
    tabela[{NaoTerminal::OUTROS_TERMOS, TipoToken::MAIORIGUAL}] = {};
    tabela[{NaoTerminal::OUTROS_TERMOS, TipoToken::MENORIGUAL}] = {};
    tabela[{NaoTerminal::OUTROS_TERMOS, TipoToken::MENOR}] = {};
    tabela[{NaoTerminal::OUTROS_TERMOS, TipoToken::MAIOR}] = {};
    tabela[{NaoTerminal::OUTROS_TERMOS, TipoToken::FECHAPARENTESES}] = {};

    //PFALSA -> ''
    tabela[{NaoTerminal::PFALSA, TipoToken::FECHACHAVES}] = {};
    tabela[{NaoTerminal::PFALSA, TipoToken::IDENTIFICADOR}] = {};
    tabela[{NaoTerminal::PFALSA, TipoToken::COUT}] = {};
    tabela[{NaoTerminal::PFALSA, TipoToken::IF}] = {};
    tabela[{NaoTerminal::PFALSA, TipoToken::WHILE}] = {};
    tabela[{NaoTerminal::PFALSA, TipoToken::DOUBLE}] = {};

    //CMDS -> CMD MAIS_CMDS
    tabela[{NaoTerminal::CMDS, TipoToken::COUT}] = {
        NaoTerminal::CMD,
        NaoTerminal::MAIS_CMDS
    };

    // CMD -> cout << id << endl
    tabela[{NaoTerminal::CMD, TipoToken::COUT}] = {
        TipoToken::COUT,
        TipoToken::DESLOCAESQUERDA,
        TipoToken::IDENTIFICADOR,
        TipoToken::DESLOCAESQUERDA,
        TipoToken::ENDL
    };

    //CMD -> id RESTO_IDENT
    tabela[{NaoTerminal::CMD, TipoToken::IDENTIFICADOR}] = {
        TipoToken::IDENTIFICADOR,
        NaoTerminal::RESTO_IDENT
    };

    //RESTO_IDENT -> = EXP_IDENT
    tabela[{NaoTerminal::RESTO_IDENT, TipoToken::ATRIBUICAO}] = {
        TipoToken::ATRIBUICAO,
        NaoTerminal::EXP_IDENT
    };

    //EXP_IDENT -> EXPRESSAO
    tabela[{NaoTerminal::EXP_IDENT, TipoToken::NUMEROREAL}] = {
        NaoTerminal::EXPRESSAO
    };

    //EXP_IDENT -> EXPRESSAO
    tabela[{NaoTerminal::EXP_IDENT, TipoToken::IDENTIFICADOR}] = {
        NaoTerminal::EXPRESSAO
    };

    // EXP_IDENT -> EXPRESSAO
    tabela[{NaoTerminal::EXP_IDENT, TipoToken::ABREPARENTESES}] = {
        NaoTerminal::EXPRESSAO
    };

    // EXPRESSAO -> TERMO OUTROS_TERMOS
    tabela[{NaoTerminal::EXPRESSAO, TipoToken::ABREPARENTESES}] = {
        NaoTerminal::TERMO,
        NaoTerminal::OUTROS_TERMOS
    };

    // EXP_IDENT -> EXPRESSAO
    tabela[{NaoTerminal::EXP_IDENT, TipoToken::SUBTRACAO}] = {
        NaoTerminal::EXPRESSAO
    };

    // EXPRESSAO -> TERMO OUTROS_TERMOS
    tabela[{NaoTerminal::EXPRESSAO, TipoToken::SUBTRACAO}] = {
        NaoTerminal::TERMO,
        NaoTerminal::OUTROS_TERMOS
    };

    // TERMO -> OP_UN FATOR MAIS_FATORES
    tabela[{NaoTerminal::TERMO, TipoToken::SUBTRACAO}] = {
        NaoTerminal::OP_UN,
        NaoTerminal::FATOR,
        NaoTerminal::MAIS_FATORES
    };

    // OP_UN -> -
    tabela[{NaoTerminal::OP_UN, TipoToken::SUBTRACAO}] = {
        TipoToken::SUBTRACAO
    };

    // TERMO -> OP_UN FATOR MAIS_FATORES
    tabela[{NaoTerminal::TERMO, TipoToken::ABREPARENTESES}] = {
        NaoTerminal::OP_UN,
        NaoTerminal::FATOR,
        NaoTerminal::MAIS_FATORES
    };

    // OP_UN -> ε
    tabela[{NaoTerminal::OP_UN, TipoToken::ABREPARENTESES}] = {};

    // FATOR -> ( EXPRESSAO )
    tabela[{NaoTerminal::FATOR, TipoToken::ABREPARENTESES}] = {
        TipoToken::ABREPARENTESES,
        NaoTerminal::EXPRESSAO,
        TipoToken::FECHAPARENTESES
    };

    // EXP_IDENT -> lerDouble ( )
    tabela[{NaoTerminal::EXP_IDENT, TipoToken::LERDOUBLE}] = {
        TipoToken::LERDOUBLE,
        TipoToken::ABREPARENTESES,
        TipoToken::FECHAPARENTESES
    };

    //EXPRESSAO -> TERMO OUTROS_TERMOS
    tabela[{NaoTerminal::EXPRESSAO, TipoToken::IDENTIFICADOR}] = {
        NaoTerminal::TERMO,
        NaoTerminal::OUTROS_TERMOS
    };

    //TERMO -> OP_UN FATOR MAIS_FATORES
    tabela[{NaoTerminal::TERMO, TipoToken::IDENTIFICADOR}] = {
        NaoTerminal::OP_UN,
        NaoTerminal::FATOR,
        NaoTerminal::MAIS_FATORES
    };

    //OP_UN -> ''
    tabela[{NaoTerminal::OP_UN, TipoToken::IDENTIFICADOR}] = {};

    //FATOR -> id
    tabela[{NaoTerminal::FATOR, TipoToken::IDENTIFICADOR}] = {
        TipoToken::IDENTIFICADOR
    };

    //EXPRESSAO -> TERMO OUTROS_TERMOS
    tabela[{NaoTerminal::EXPRESSAO, TipoToken::NUMEROREAL}] = {
        NaoTerminal::TERMO,
        NaoTerminal::OUTROS_TERMOS
    };

    //TERMO -> OP_UN FATOR MAIS_FATORES
    tabela[{NaoTerminal::TERMO, TipoToken::NUMEROREAL}] = {
        NaoTerminal::OP_UN,
        NaoTerminal::FATOR,
        NaoTerminal::MAIS_FATORES
    };

    //OP_UN -> ''
    tabela[{NaoTerminal::OP_UN, TipoToken::NUMEROREAL}] = {};

    //FATOR -> numero_real
    tabela[{NaoTerminal::FATOR, TipoToken::NUMEROREAL}] = {
        TipoToken::NUMEROREAL
    };

    //MAIS_FATORES -> ''
    tabela[{NaoTerminal::MAIS_FATORES, TipoToken::PONTOEVIRGULA}] = {}; 

    // MAIS_FATORES -> OP_MUL FATOR MAIS_FATORES
    tabela[{NaoTerminal::MAIS_FATORES, TipoToken::MULTIPLICACAO}] = {
        NaoTerminal::OP_MUL,
        NaoTerminal::FATOR,
        NaoTerminal::MAIS_FATORES
    };

    //MAIS_FATORES -> OP_MUL FATOR MAIS_FATORES
    tabela[{NaoTerminal::MAIS_FATORES, TipoToken::DIVISAO}] = {
        NaoTerminal::OP_MUL,
        NaoTerminal::FATOR,
        NaoTerminal::MAIS_FATORES
    };

    // MAIS_FATORES -> ''
    tabela[{NaoTerminal::MAIS_FATORES, TipoToken::SOMA}] = {};
    tabela[{NaoTerminal::MAIS_FATORES, TipoToken::SUBTRACAO}] = {};

    // OUTROS_TERMOS -> OP_AD TERMO OUTROS_TERMOS
    tabela[{NaoTerminal::OUTROS_TERMOS, TipoToken::SOMA}] = {
        NaoTerminal::OP_AD,
        NaoTerminal::TERMO,
        NaoTerminal::OUTROS_TERMOS
    };

    tabela[{NaoTerminal::OUTROS_TERMOS, TipoToken::SUBTRACAO}] = {
        NaoTerminal::OP_AD,
        NaoTerminal::TERMO,
        NaoTerminal::OUTROS_TERMOS
    };

    // OP_AD -> +
    tabela[{NaoTerminal::OP_AD, TipoToken::SOMA}] = {
        TipoToken::SOMA
    };

    // OP_AD -> -
    tabela[{NaoTerminal::OP_AD, TipoToken::SUBTRACAO}] = {
        TipoToken::SUBTRACAO
    };

    // OP_MUL -> *
    tabela[{NaoTerminal::OP_MUL, TipoToken::MULTIPLICACAO}] = {
        TipoToken::MULTIPLICACAO
    };

    // OP_MUL -> /
    tabela[{NaoTerminal::OP_MUL, TipoToken::DIVISAO}] = {
        TipoToken::DIVISAO
    };

    //OUTROS_TERMOS -> ''
    tabela[{NaoTerminal::OUTROS_TERMOS, TipoToken::PONTOEVIRGULA}] = {};

    //CMDS -> DC
    tabela[{NaoTerminal::CMDS, TipoToken::DOUBLE}] = {
        //desempilha CMDS e empilha DC
        NaoTerminal::DC
    };

    //DC -> VAR MAIS_CMDS
    tabela[{NaoTerminal::DC, TipoToken::DOUBLE}] = {
        NaoTerminal::VAR,
        NaoTerminal::MAIS_CMDS
    };

    //VAR -> TIPO VARS
    tabela[{NaoTerminal::VAR, TipoToken::DOUBLE}] = {
        NaoTerminal::TIPO,
        NaoTerminal::VARS
    };

    //TIPO -> double
    tabela[{NaoTerminal::TIPO, TipoToken::DOUBLE}] = {
        TipoToken::DOUBLE
    };

    //VARS -> id MAIS_VAR
    tabela[{NaoTerminal::VARS, TipoToken::IDENTIFICADOR}] = {
        TipoToken::IDENTIFICADOR,
        NaoTerminal::MAIS_VAR
    };

    tabela[{NaoTerminal::MAIS_VAR, TipoToken::VIRGULA}] = {
        TipoToken::VIRGULA,
        NaoTerminal::VARS
    };

    //MAIS_VAR -> vazio
    tabela[{NaoTerminal::MAIS_VAR, TipoToken::PONTOEVIRGULA}] = {};

    //MAIS_CMDS -> ; CMDS
    tabela[{NaoTerminal::MAIS_CMDS, TipoToken::PONTOEVIRGULA}] = {
        TipoToken::PONTOEVIRGULA,
        NaoTerminal::CMDS
    };

    return tabela;
}

//Instanciando um sintatico que recebe o lexer e guarda uma referência a ele para buscar tokens
Sintatico::Sintatico(Lexico& lexico) : lexico(lexico) {}

//implementando o método Analisar()
void Sintatico::analisar() {

    //Criando uma tabelaM e guardando em tabela do tipo TabelaM
    TabelaM tabela = criarTabelaM();

    //declarando a pilha, vetor de simbolos
    std::vector<Simbolo> pilha;

    //empilhando fim arquivo
    pilha.push_back(TipoToken::FIMARQUIVO);

    //empilhando inicio do programa
    pilha.push_back(NaoTerminal::PROG);

    //buscando o proximo token e guardando em atual
    Token atual = lexico.proximoToken();

    //enquanto a pilha não estiver vazia
    while (!pilha.empty()) {
        
        //guardando o valor do topo da pilha e guardando em topo do tipo Simbolo
        Simbolo topo = pilha.back();

        //desempilhando o topo
        pilha.pop_back();

        // DESEMPILHO PRIMEIRO, COMPARO DEPOIS
        // se for terminal, checo se o terminal do topo e o terminal da entrada são do mesmo tipo, se não forem, é erro sintático
        /* se for um não terminal, procuro a regra na tabela cuja chave seja M[NaoTerminal][TipoToken], se eu não encontrar é erro sintático 
            se eu encontrar, como eu já desempilhei o não terminal, apenas empilho a regra um simbolo de cada vez ao contrário*/

        //checando se o topo da pilha é um terminal
        if (std::holds_alternative<TipoToken>(topo)) {
            
            //salvando o tipo de token esperado
            TipoToken esperado = std::get<TipoToken>(topo);

            //se o token esperado não for do mesmo tipo do atual exemplo: PONTOEVIRGULA no topo e PONTOEVIRGULA na entrada, é um erro sintático
            if (esperado != atual.tipo) {
                throw std::runtime_error(
                    std::string("Erro sintatico:\n") +
                    "linha " + std::to_string(atual.linha) +
                    ", coluna " + std::to_string(atual.coluna) + "\n" +
                    "encontrado " + descricaoToken(atual) +
                    "; esperado " + token(esperado) + "."
                );
            } 

            // se o token esperado for FIMARQUIVO, fim da execução
            if (esperado == TipoToken::FIMARQUIVO) {
                return;
            }

            // iterando o valor de atual para o proximoToken
            atual = lexico.proximoToken();
        
        //se o que estiver no topo não for um terminal
        } else {

            //guardo o valor do topo em naoTerminal do tipo NaoTerminal
            NaoTerminal naoTerminal = std::get<NaoTerminal>(topo);

            //auto permite o compilador descobrir o tipo de marcador, find procura uma chave no map
            //exemplo de ChaveTabela: {NaoTerminal::PROG, TipoToken::INCLUDE} M[PROG][#include], esse par de chaves localiza a produção na tabela
            //quando meu não terminal é PROG e meu terminal é #include, qual a regra e a produção?
            auto marcador = tabela.find({naoTerminal, atual.tipo});

            //se o marcador chegou ao fim da tabela foi porque não encontrou a ChaveTabela que estava procurando
            if (marcador == tabela.end()) {
                
                throw std::runtime_error(
                    std::string("Erro sintatico:\n") +
                    "linha " + std::to_string(atual.linha) + ", coluna " + std::to_string(atual.coluna) + "\n" +
                    "encontrado " + descricaoToken(atual) +
                    "; \n" + "Não há produção válida!"
                );
            }

            //caso o marcador tenha encontrado o par, qual a produção que essa célula da tabela indica?
            //marcador->first é a chave, second é o valor, neste caso a Producao, logo, salvo em producao do tipo Producao
            const Producao& producao = marcador->second;

            //de, rbegin() começa pelo último elemento da produção, já que empilharemos a produção ao contrário
            //até, rend() que marca o fim da iteração reversa
            //iterando simbolo
            for (auto simbolo = producao.rbegin(); simbolo != producao.rend(); ++simbolo) {

                //empilha o simbolo
                pilha.push_back(*simbolo);
            }
        }
    }
};
