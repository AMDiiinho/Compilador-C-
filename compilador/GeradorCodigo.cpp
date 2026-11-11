#include "GeradorCodigoInterface.hpp"

#include <stdexcept>
#include <string>

std::vector<std::string> GeradorCodigo::gerar(const Programa& programa, const TabelaSimbolos& tabela) const {

    std::vector<std::string> codigo;

    // INPP inicia a execução e ALME reserva espaço para as variáveis declaradas.
    codigo.push_back("INPP");
    codigo.push_back("ALME " + std::to_string(tabela.quantidade()));

    // Os comandos são gerados na mesma ordem em que aparecem no programa.
    gerarComandos(programa.comandos, tabela, codigo);

    // PARA encerra a execução da máquina após o último comando.
    codigo.push_back("PARA");

    return codigo;
}




// Gera o código de cada comando pertencente a um bloco.
void GeradorCodigo::gerarComandos(const std::vector<ComandoPtr>& comandos, const TabelaSimbolos& tabela, std::vector<std::string>& codigo) {

    for (const ComandoPtr& comando : comandos) {

        gerarComando(*comando, tabela, codigo);
    };
}





// Traduz um comando da AST para uma ou mais instruções da MaqHipo.
void GeradorCodigo::gerarComando(const Comando& comando, const TabelaSimbolos& tabela, std::vector<std::string>& codigo) {

    // A declaração já foi registrada na tabela de símbolos e não gera instrução.
    if (dynamic_cast<const Declaracao*>(&comando) != nullptr) {

        return;
    }

    // Primeiro calcula a expressão e depois armazena o resultado no destino.
    if (const auto* atribuicao = dynamic_cast<const Atribuicao*>(&comando)) {

        gerarExpressao(*atribuicao->expressao, tabela, codigo);

        const int endereco = tabela.obter(atribuicao->destino).endereco;

        codigo.push_back("ARMZ " + std::to_string(endereco));
        return;
    }


    // CRVL carrega o valor da variável e IMPR envia esse valor para a saída.
    if (const auto* impressao = dynamic_cast<const Print*>(&comando)) {

        const int endereco = tabela.obter(impressao->id).endereco;

        codigo.push_back("CRVL " + std::to_string(endereco));
        codigo.push_back("IMPR");
        return;
    }

    // A condição deixa um valor lógico na pilha para DSVF testar.
    if (const auto* se = dynamic_cast<const Se*>(&comando)) {

        gerarCondicao(se->condicao, tabela, codigo);

        // O destino do salto falso será preenchido após a geração do bloco Entao.
        const std::size_t saltoFalso = codigo.size();
        codigo.push_back("DSVF 0");

        gerarComandos(se->entao, tabela, codigo);

        if (se->senao.empty()) {

            // Sem Senao, o salto aponta diretamente para a próxima instrução.
            gerarSalto(codigo, saltoFalso, codigo.size());
            return;
        };

        // Com Senao, o fluxo verdadeiro precisa saltar por cima desse bloco.
        const std::size_t saltoFim = codigo.size();
        codigo.push_back("DSVI 0");

        gerarSalto(codigo, saltoFalso, codigo.size());

        gerarComandos(se->senao, tabela, codigo);

        gerarSalto(codigo, saltoFim, codigo.size());
        return;
    };


    // O laço retorna ao seu início enquanto a condição continuar verdadeira.
    if (const auto* enquanto = dynamic_cast<const Enquanto*>(&comando)) {

        const std::size_t inicio = codigo.size();

        gerarCondicao(enquanto->condicao, tabela, codigo);

        // O destino do salto falso será definido depois que o corpo for gerado.
        const std::size_t saltoFim = codigo.size();
        codigo.push_back("DSVF 0");

        gerarComandos(enquanto->corpo, tabela, codigo);
        codigo.push_back("DSVI " + std::to_string(inicio));

        gerarSalto(codigo, saltoFim, codigo.size());
        return;
    }

    throw std::runtime_error("Comando não suportado!!");

};



// Traduz uma expressão e deixa o seu resultado no topo da pilha de dados.
void GeradorCodigo::gerarExpressao(const Expressao& expressao, const TabelaSimbolos& tabela, std::vector<std::string>& codigo) {

    // Constantes são carregadas diretamente para a pilha.
    if (const auto* numero = dynamic_cast<const Numero*>(&expressao)) {

        codigo.push_back("CRCT " + numero->valor);
        return;
    };


    // A tabela de símbolos informa o endereço da variável na memória.
    if (const auto* identificador = dynamic_cast<const Identificador*>(&expressao)) {

        const int endereco = tabela.obter(identificador->id).endereco;
        codigo.push_back("CRVL " + std::to_string(endereco));
        return;
    };


    // LEIT lê um valor e o coloca no topo da pilha.
    if (dynamic_cast<const Leitura*> (&expressao) != nullptr) {

        codigo.push_back("LEIT");
        return;
    };



    // O operando é calculado primeiro; INVE então inverte o seu sinal.
    if (const auto* unaria = dynamic_cast<const ExpressaoUnaria*>(&expressao)) {

        gerarExpressao(*unaria->operando, tabela, codigo);
        codigo.push_back("INVE");
        return;
    };




    // As duas expressões são avaliadas antes da operação binária.
    if (const auto* binaria = dynamic_cast<const ExpressaoBinaria*>(&expressao)) {

        gerarExpressao(*binaria->esquerda, tabela, codigo);
        gerarExpressao(*binaria->direita, tabela, codigo);

        // Cada operador binário possui uma instrução correspondente na MaqHipo.
        switch (binaria->operador){

            case OperadorBinario::Soma:
                codigo.push_back("SOMA");
                return;

            case OperadorBinario::Subtracao:
                codigo.push_back("SUBT");
                return;

            case OperadorBinario::Multiplicacao:
                codigo.push_back("MULT");
                return;

            case OperadorBinario::Divisao:
                codigo.push_back("DIVI");
                return;

        }
    }

    throw std::runtime_error("Expressão não suportada!!");
}



// Gera as instruções que comparam as duas expressões de uma condição.
void GeradorCodigo::gerarCondicao(const Condicao& condicao, const TabelaSimbolos& tabela, std::vector<std::string>& codigo) {

    // As expressões deixam seus valores na pilha para a comparação consumir.
    gerarExpressao(*condicao.esquerda, tabela, codigo);
    gerarExpressao(*condicao.direita, tabela, codigo);

    // A instrução escolhida depende do operador relacional da condição.
    switch (condicao.operador) {
    
        case OperadorRelacional::Igual:
            
            codigo.push_back("CPIG");
            return;

        case OperadorRelacional::Diferente:
            
            codigo.push_back("CDES");
            return;

        case OperadorRelacional::Maior:
            
            codigo.push_back("CPMA");
            return;

        case OperadorRelacional::Menor:
            
            codigo.push_back("CPME");
            return;

        case OperadorRelacional::MaiorIgual:
            
            codigo.push_back("CMAI");
            return;

        case OperadorRelacional::MenorIgual:
            
            codigo.push_back("CPMI");
            return;
    };

    throw std::runtime_error("Operador relacional nao suportado.");
}





// Substitui o destino provisório de um salto pelo endereço real da instrução.
// O salto é criado antes porque o tamanho do bloco ainda não é conhecido.
void GeradorCodigo::gerarSalto(std::vector<std::string>& codigo, std::size_t indice, std::size_t destino) {

    const std::string instrucao = codigo.at(indice).substr(0, codigo.at(indice).find(' '));
    codigo.at(indice) = instrucao + " " + std::to_string(destino);
}
