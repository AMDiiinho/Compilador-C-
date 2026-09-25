#include "SemanticoInterface.hpp"


ErroSemantico::ErroSemantico(const std::string& mensagem) : std::runtime_error("Erro semantico: " + mensagem) {}

TabelaSimbolos Semantico::analisar(const Programa& programa) const {
    
    //iniciando uma tabela de simbolos e enviando os comandos do programa para anaálise
    TabelaSimbolos tabela;
    analisarComandos(programa.comandos, tabela);
    return tabela;
}

// recebo um vetor de Ponteiros para os nós da árvore que contém os comandos e a tabela
void Semantico::analisarComandos(const std::vector<ComandoPtr>& comandos, TabelaSimbolos& tabela) {
    
    for (const ComandoPtr& comando : comandos) {
        
        //para cada comando em comandos, envio o comando para análise
        analisarComando(*comando, tabela);
    }
}

void Semantico::analisarComando(const Comando& comando, TabelaSimbolos& tabela) {
    
    //checo se o objeto que está no endereço de comando pode ser interpretado como uma Declaração, se puder, retorno um ponteiro para o mesmo objeto
    if (const auto* declaracao = dynamic_cast<const Declaracao*>(&comando)) {
        
        // para cada id em declaracao
        for (const std::string& id : declaracao->ids) {
            
            // tento inserir o id na tabela com um endereço, caso ele tente inserir dois ids iguais, é retorno o erro
            if (!tabela.declarar(id)) {
                
                throw ErroSemantico("variavel '" + id + "' redeclarada.");
            }
        }
        return;
    }

    // checando se o objeto no endereço de comando pode ser interpretado como uma Atribuição, se puder, retorno um ponteiro para o mesmo objeto
    if (const auto* atribuicao = dynamic_cast<const Atribuicao*>(&comando)) {
        
        //checo se o destino está presente na tabela
        exigirDeclarada(atribuicao->destino, tabela);

        //checo se é uma expressao semanticamente válida
        analisarExpressao(*atribuicao->expressao, tabela);
        return;
    }

    if (const auto* impressao = dynamic_cast<const Print*>(&comando)) {
        
        //checo se o id que será imprimido existe na tabela
        exigirDeclarada(impressao->id, tabela);
        return;
    }

    if (const auto* se = dynamic_cast<const Se*>(&comando)) {

        //checo se a condição é semanticamente valida
        analisarCondicao(se->condicao, tabela);
        
        //checando de o vetor de comandos para então e para senão são semanticamente válidos
        analisarComandos(se->entao, tabela);
        analisarComandos(se->senao, tabela);
        return;
    }
    

    if (const auto* enquanto = dynamic_cast<const Enquanto*>(&comando)) {
        
        //checando se a condição é semanticamente válida
        analisarCondicao(enquanto->condicao, tabela);

        //checando se o vetor de comandos corpo é semanticamente válido
        analisarComandos(enquanto->corpo, tabela);
        return;
    }

    throw ErroSemantico("comando desconhecido na AST.");
}


void Semantico::analisarCondicao(const Condicao& condicao, TabelaSimbolos& tabela) {
    //analisarCondicao receberá algo como (id < id), (id != 2), (2 > 3), então envio o trecho da esquerda e o da direita separadamente
    //para a analise
    analisarExpressao(*condicao.esquerda, tabela);
    analisarExpressao(*condicao.direita, tabela);
}

void Semantico::analisarExpressao(const Expressao& expressao, TabelaSimbolos& tabela) {
    
    //checando se o objeto em &expressão pode ser interpretado como um Identificador, se puder, retorno um ponteiro para o mesmo objeto
    if (const auto* identificador = dynamic_cast<const Identificador*>(&expressao)) {
        
        //caso existe um id na expressão, preciso verificar se ela foi declarada
        exigirDeclarada(identificador->id, tabela);
        return;
    }

    //checando se consegui criar um ponteiro de numero válido baseado no endereço de expressao, 
    //ou se um casting de &expressão para um ponteiro de leitura criou um ponteiro válido
    if (dynamic_cast<const Numero*>(&expressao) != nullptr || dynamic_cast<const Leitura*>(&expressao) != nullptr) {
        
        return;
    }

    //checando se baseado no endereço de expressão, consigo criar um ponteiro válido de ExpressaoUnaria
    if (const auto* unaria = dynamic_cast<const ExpressaoUnaria*>(&expressao)) {
        
        //caso eu consiga, passo o operando para análise que pode ser um id ou um número
        //Ex: -(a + b), o operando é (a + b) que em analisarExpressão cairá no if que checa uma expressaoBinaria e tratada com direita e esquerda recursivamente
        //Ex: -a, o operando é a, um id, cairá no if que checa se é um id
        analisarExpressao(*unaria->operando, tabela);
        return;
    }

    //checando se consigo criar um ponteiro para expressaoBinaria com base no endereço da expressão
    if (const auto* binaria = dynamic_cast<const ExpressaoBinaria*>(&expressao)) {
        
        //chamo analisarExpressao recursivamente para cada membro
        //Ex: a + b
        // a --> esquerda
        // b --> direita
        // os dois são ids, cairá no if que checa se é um id, e se for, checará se eles estão presentes na tabela, ou seja, se foram declarados anteriormente
        analisarExpressao(*binaria->esquerda, tabela);
        analisarExpressao(*binaria->direita, tabela);
        return;
    }

    throw ErroSemantico("expressao desconhecida na AST.");
}

void Semantico::exigirDeclarada(const std::string& id, const TabelaSimbolos& tabela) {
    
    //checa se o id existe na tabela
    if (!tabela.contem(id)) {
        
        throw ErroSemantico("variavel '" + id + "' usada antes de ser declarada.");
    }
}
