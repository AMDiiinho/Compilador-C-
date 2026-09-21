#include "ConversorASTInterface.hpp"

#include <stdexcept>

// Retornando o filho do nó pela posicao que ele ocupa na producao
// Ex: filho (no, 0) acessa o primeiro símbolo da producao
const No& ConversorAST::filho(const No& no, std::size_t indice) {
    
    return *no.filhos.at(indice);
}

// Retornando o token armazenado em um nó terminal
// Obtém o nome de um id, Ex: tokenNo.lexema
// tokenNo retorna um valor do tipo Token
const Token& ConversorAST::tokenNo(const No& no) {

    if (!no.token.has_value()) {

        throw std::logic_error("Nó terminal sem token reconhecido.");
    }

    return *no.token;
}

// Verifica se o nó contém um não terminal específico.
// holds_alternative verifica qual dos alternativos está presente
bool ConversorAST::ehNaoTerminal(const No& no, NaoTerminal naoTerminal) {
    
    // verifica se no.simbolo guarda um valor do tipo NaoTerminal, depois checa se esse não terminal é do mesmo tipo do
    //naoterminal recebido como parametro
    return std::holds_alternative<NaoTerminal>(no.simbolo) && std::get<NaoTerminal>(no.simbolo) == naoTerminal;
}

// O conversor receberá o Nó raiz (PROG) e devolverá um tipo programa, que é um vetor de comandos
Programa ConversorAST::converter(const No& raiz) const {

    // checando se o nó raiz é um não terminal e se ele é do tipo PROG
    if(!ehNaoTerminal(raiz, NaoTerminal::PROG)) {

        throw std::logic_error("A raiz da arvore sintatica deveria ser PROG.");
    };

    //instanciando um programa
    Programa programa;

    //PROG produz os filhos {#incluvde, <, iostream, >, usign...}, CMDS é o símbolo de índice 13 que gera todo o programa, então acesso ele
    // porque apartir dele é que vou fazer as conversoes recursivamente;
    programa.comandos = converterComandos(filho(raiz, 13));

    return programa;
};




//Convertendo operadores binários
OperadorBinario ConversorAST::converterOperadorBinario(TipoToken tipo) {
    
    switch (tipo) {
        
        case TipoToken::SOMA:
            return OperadorBinario::Soma;

        case TipoToken::SUBTRACAO:
            return OperadorBinario::Subtracao;

        case TipoToken::MULTIPLICACAO:
            return OperadorBinario::Multiplicacao;

        case TipoToken::DIVISAO:
            return OperadorBinario::Divisao;

        default:
            throw std::logic_error("Token não é um operador aritmético.");
    }

};




// convertendo operadores relacionais
OperadorRelacional ConversorAST::converterOperacaoRelacional(TipoToken tipo) {

    switch (tipo) {

        case TipoToken::IGUAL:
            return OperadorRelacional::Igual;

        case TipoToken::DIFERENTE:
            return OperadorRelacional::Diferente;

        case TipoToken::MAIOR:
            return OperadorRelacional::Maior;

        case TipoToken::MENOR:
            return OperadorRelacional::Menor;

        case TipoToken::MAIORIGUAL:
            return OperadorRelacional::MaiorIgual;

        case TipoToken::MENORIGUAL:
            return OperadorRelacional::MenorIgual;

        default:

            throw std::logic_error("Token não é um operador relacional.");
    }
}





// Convertendo fatores
ExpressaoPtr ConversorAST::converterFator(const No& fator) const {

    //acesso o primeiro filho
    const No& primeiroFilho = filho(fator, 0);

    //acesso o tipo do token do primeiro filho
    const TipoToken tipo = tokenNo(primeiroFilho).tipo;

    //se o tipo do fator for um id como "a"
    if (tipo == TipoToken::IDENTIFICADOR) {

        //crio um Nó Identificador(id)
        return std::make_unique<Identificador>(tokenNo(primeiroFilho).lexema);
    }

    //se o tipo for um numero
    if (tipo == TipoToken::NUMEROREAL) {

        //crio um Nó Numero(num)
        return std::make_unique<Numero>(tokenNo(primeiroFilho).lexema);
    }


    // FATOR -> ( EXPRESSAO )
    /* FATOR
       |__ (
       |__ EXPRESSAO
       |    |__ .... a + 2 * c ....
       |___)

    */
    if (tipo == TipoToken::ABREPARENTESES) {

        //então eu retorno a conversão do próximo nó filho que é EXPRESSAO que contém toda a subarvore de operações e envia para converterExpressao
        return converterExpressao(filho(fator, 1));
    };

    throw std::logic_error("FATOR inválido na árvore sintática.");
};





//Convertendo termos
//TERMO -> OP_UN FATOR MAIS_FATORES
ExpressaoPtr ConversorAST::converterTermo(const No& termo) const {

    // envio o indice 1 para converterFator porque é esse o indice de FATOR na produção de TERMO
    ExpressaoPtr resultado = converterFator(filho(termo, 1));

    // OP_UN -
    // aqui eu checo se o nó de indice 0 de termo possui filhos, se possuir, é porque existe um Operador unário -; Ex: -a * b
    if (!filho(termo, 0).filhos.empty()) {

        //se tiver filhos, eu crio um Nó contendo o -
        resultado = std::make_unique<ExpressaoUnaria>(OperadorUnario::Menos, std::move(resultado));
    };

    // para finalizar a conversão do termo, eu mando MAIS_FATORES de indice 2 para a conversão recursivamente e retorno o resultado 
    return converterMaisFatores(filho(termo, 2), std::move(resultado));
}





// Convertendo mais fatores
//MAIS FATORES -> OP_MUL FATOR MAIS_FATORES
// eu passo o esquerda porque estou passando a árvore que construi até agora recursivamente, até que eu retorne esquerda de fato, ou seja
//não há mais fatores, cheguei no final dos filhos e filhos dos filhos
ExpressaoPtr ConversorAST::converterMaisFatores(const No& maisFatores, ExpressaoPtr esquerda) const {

    //MAIS FATORES -> ''
    if (maisFatores.filhos.empty()) {
        
        //caso não existam mais fatores, eu retorno a árvore que foi construida até o momento
        return esquerda;
    }
    
    //acesso o nó OP_MUL, que é o indice 0 de maisFatores
    const No& opMul = filho(maisFatores, 0);

    //acesso o tipo de operator
    const TipoToken tipoOperador = tokenNo(filho(opMul, 0)).tipo;

    //envio o indice 1 da produção de MAIS_FATORES (FATOR), para a conversao
    ExpressaoPtr direita = converterFator(filho(maisFatores, 1));

    //construo a nova arvore criando um novo nó ExpressaoBinaria com os filhos esquerda, tipoOperador e direita
    ExpressaoPtr novaEsquerda = std::make_unique<ExpressaoBinaria>(
        std::move(esquerda), converterOperadorBinario(tipoOperador), std::move(direita)
    );

    //chamo recursivamente converterMaisFatores() para o índice 2 da produção de MAIS_FATORES que é MAIS_FATORES
    return converterMaisFatores(filho(maisFatores, 2), std::move(novaEsquerda));
    
}




// convertendo mais termos
// OUTROS_TERMOS ->OP_AD TERMO OUTROS TERMOS
ExpressaoPtr ConversorAST::converterOutrosTermos(const No& outrosTermos, ExpressaoPtr esquerda) const {

    // OUTROS_TERMOS -> ''
    // se eu já cpnverti todos os termos, então eu retorno a árvore que eu construi recursivamente
    if (outrosTermos.filhos.empty()) {

        return esquerda;
    }

    // OUTROS_TERMOS -> OP_AD TERMO OUTROS_TERMOS
    // Pegando o nó filho que representa o operador + ou -
    const No& opAd = filho(outrosTermos, 0);

    //pegando o tipo do operador ADICAO ou SUBTRACAO
    const TipoToken tipoOperador = tokenNo(filho(opAd, 0)).tipo;

    // converto o indice 1 da produção, que é TERMO
    ExpressaoPtr direita = converterTermo(filho(outrosTermos, 1));

    // construo o novo trecho da árvore contendo esquerda, o operador e direita
    ExpressaoPtr novaEsquerda = std::make_unique<ExpressaoBinaria>(
        std::move(esquerda), converterOperadorBinario(tipoOperador), std::move(direita)
    );

    //chamo converterOutrosTermos recursivamente para o indice 2 da produção que é OUTROS_TERMOS e passo a árvore q eu construi até agora
    return converterOutrosTermos(filho(outrosTermos, 2), std::move(novaEsquerda));

}




// convertendo expressoes
// EXPRESSAO -> TERMO OUTROS_TERMOS
ExpressaoPtr ConversorAST::converterExpressao(const No& exp) const {

    //mando o filho TERMO da para a conversão
    ExpressaoPtr resultado = converterTermo(filho(exp, 0));

    //mando o filho OUTROS_TERMOS para a conversao
    return converterOutrosTermos(filho(exp, 1), std::move(resultado));
}




//convertendo expressoes com ids
ExpressaoPtr ConversorAST::converterExpId(const No& expId) const {

    //as regras de EXP_IDENT sempre tem somente 1 produção, então já pego a produção de indice 0
    const No& primeiroFilho = filho(expId, 0);

    // EXP_IDENT -> lerDouble ( )
    // caso ele produza lerDouble
    // verifica se no.simbolo guarda um valor do tipo TipoToken, depois checa se esse token é do mesmo tipo do token que está no nó
    if (std::holds_alternative<TipoToken>(primeiroFilho.simbolo) && tokenNo(primeiroFilho).tipo == TipoToken::LERDOUBLE) {
        
        // se for, é o caso de lerDouble(), então crio um Nó de Leitura
        return std::make_unique<Leitura>();
    };
    

    // EXP_IDENT -> EXPRESSAO
    // envio o filho de EXP_IDENT para conversão
    return converterExpressao(primeiroFilho);
    
};




//convertendo condicoes
Condicao ConversorAST::converterCondicao(const No& condicao) const {

    // CONDICAO -> EXPRESSAO RELACAO EXPRESSAO
    ExpressaoPtr esquerda = converterExpressao(filho(condicao, 0));

    const No& relacao = filho(condicao, 1);
    const TipoToken tipoOperador = tokenNo(filho(relacao, 0)).tipo;

    ExpressaoPtr direita = converterExpressao(filho(condicao, 2));

    Condicao resultado;
    resultado.esquerda = std::move(esquerda);
    resultado.operador = converterOperacaoRelacional(tipoOperador);
    resultado.direita = std::move(direita);

    return resultado;
};





void ConversorAST::coletarIds(const No& vars, std::vector<std::string>& ids) {

    //VARS -> id MAIS_VAR
    //caso: dobule a, b, c

    //tokenNo retorna o token armazenado em um nó terminal, como o retorno é um Token consigo acessar o nome dele com .lexema
    //então eu adiciono esse lexema no vetor de strings ids
    ids.push_back(tokenNo(filho(vars, 0)).lexema);

    //acesso o MAIS_VAR da produção de VARS, indice 1
    const No& maisVar = filho(vars, 1);

    //MAIS_VAR -> ''
    //se eu não tiver mais variaveis, retorno
    if (maisVar.filhos.empty()) {
        return;
    }

    //MAIS_VAR -> , VARS
    //se eu tiver mais variaveis, chamo a propria funcao recursivamente, passando o indice 1 (VARS), para ser convertido e o vetor ids que
    //construi até o momento
    coletarIds(filho(maisVar, 1), ids);
}




ComandoPtr ConversorAST::converterDeclaracao(const No& var) const {

    //VAR -> TIPO VARS
    std::vector<std::string> ids;

    coletarIds(filho(var, 1), ids);

    return std::make_unique<Declaracao>(std::move(ids));

}




ComandoPtr ConversorAST::converterComando(const No& cmd) const {

    const No& primeiroFilho = filho(cmd, 0);

    //CMD -> cout << id << endl
    if (std::holds_alternative<TipoToken>(primeiroFilho.simbolo) && tokenNo(primeiroFilho).tipo == TipoToken::COUT) {

        return std::make_unique<Print>(tokenNo(filho(cmd, 2)).lexema);
    }

    //CMD -> id RESTO_IDENT
    const std::string destino = tokenNo(primeiroFilho).lexema;

    const No& restoId = filho(cmd, 1);

    //RESTO_IDENT -> = EXP_IDENT
    return std::make_unique<Atribuicao>(destino, converterExpId(filho(restoId, 1)));
}






// convertendo comandos
ComandoPtr ConversorAST::converterComandoCondicional(const No& cmdCond) const {

    const TipoToken tipoComando = tokenNo(filho(cmdCond, 0)).tipo;

    // CMD_COND -> while ( CONDICAO ) { CMDS }
    if (tipoComando == TipoToken::WHILE) {
        
        //criando um objeto enquanto na memória dinamica, auto identifica como um unique_ptr
        auto enquanto = std::make_unique<Enquanto>();

        enquanto->condicao = converterCondicao(filho(cmdCond, 2));
        enquanto->corpo = converterComandos(filho(cmdCond, 5));

        return enquanto;
    }

    //CMD_COND -> if ( CONDICAO ) { CMDS } PFALSA
    if (tipoComando == TipoToken::IF) {
        
        auto se = std::make_unique<Se>();

        se->condicao = converterCondicao(filho(cmdCond, 2));
        se->entao = converterComandos(filho(cmdCond, 5));

        const No& pfalsa = filho(cmdCond, 7);

        // PFALSA -> else { CMDS }
        if (!pfalsa.filhos.empty()) {

            se->senao = converterComandos(filho(pfalsa, 2));
        }

        return se;
    }

    throw std::logic_error("CMD_COND inválido na árvore sintática.");
}





void ConversorAST::adicionarComandos(std::vector<ComandoPtr>& destino, std::vector<ComandoPtr>origem) {

    // para cada comando no vetor de origem
    for (ComandoPtr& comando : origem) {

        //coloco o comando em destino
        destino.push_back(std::move(comando));
    }
    
}




//criando o vetor de comandos
std::vector<ComandoPtr> ConversorAST::converterDc(const No& dc) const {

    //DC -> VAR MAIS_CMDS
    std::vector<ComandoPtr> comandos;

    comandos.push_back(converterDeclaracao(filho(dc, 0)));

    adicionarComandos(comandos, converterMaisComandos(filho(dc, 1)));

    return comandos;
}





std::vector<ComandoPtr> ConversorAST::converterMaisComandos(const No& maisCmds) const {

    //MAIS_CMDS -> ; CMDS
    return converterComandos(filho(maisCmds, 1));
}





std::vector<ComandoPtr> ConversorAST::converterComandos(const No& cmds) const{

    std::vector<ComandoPtr> comandos;

    // CMDS -> ''
    if (cmds.filhos.empty()) {

        return comandos;
    }

    const No& primeiroFilho = filho(cmds, 0);

    //CMDS -> CMD MAIS_CMDS
    if(ehNaoTerminal(primeiroFilho, NaoTerminal::CMD)) {

        comandos.push_back(converterComando(primeiroFilho));

        adicionarComandos(comandos, converterMaisComandos(filho(cmds, 1)));

        return comandos;
    }

    // CMDS -> CMD_COND CMDS
    if (ehNaoTerminal(primeiroFilho, NaoTerminal::CMD_COND)) {

        comandos.push_back(converterComandoCondicional(primeiroFilho));

        adicionarComandos(comandos, converterComandos(filho(cmds, 1)));

        return comandos;
    }

    // CMDS -> DC
    if (ehNaoTerminal(primeiroFilho, NaoTerminal::DC)) {

        return converterDc(primeiroFilho);
    }

    throw std::logic_error("CMDS inválido na árvore sintática.");

}
