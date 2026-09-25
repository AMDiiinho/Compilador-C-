#include "GeradorCodigoInterface.hpp"

#include <stdexcept>
#include <string>

std::vector<std::string> GeradorCodigo::gerar(const Programa& programa, const TabelaSimbolos& tabela) const {

    std::vector<std::string> codigo;

    codigo.push_back("INPP");
    codigo.push_back("ALME " + std::to_string(tabela.quantidade()));

    gerarComandos(programa.comandos, tabela, codigo);

    codigo.push_back("PARA");

    return codigo;
}




void GeradorCodigo::gerarComandos(const std::vector<ComandoPtr>& comandos, const TabelaSimbolos& tabela, std::vector<std::string>& codigo) {

    for (const ComandoPtr& comando : comandos) {

        gerarComando(*comando, tabela, codigo);
    };
}





void GeradorCodigo::gerarComando(const Comando& comando, const TabelaSimbolos& tabela, std::vector<std::string>& codigo) {

    if (dynamic_cast<const Declaracao*>(&comando) != nullptr) {

        return;
    }

    if (const auto* atribuicao = dynamic_cast<const Atribuicao*>(&comando)) {

        gerarExpressao(*atribuicao->expressao, tabela, codigo);

        const int endereco = tabela.obter(atribuicao->destino).endereco;

        codigo.push_back("ARMZ " + std::to_string(endereco));
        return;
    }


    if (const auto* impressao = dynamic_cast<const Print*>(&comando)) {

        const int endereco = tabela.obter(impressao->id).endereco;

        codigo.push_back("CRVL " + std::to_string(endereco));
        codigo.push_back("IMPR");
        return;
    }

    if (const auto* se = dynamic_cast<const Se*>(&comando)) {

        gerarCondicao(se->condicao, tabela, codigo);

        const std::size_t saltoFalso = codigo.size();
        codigo.push_back("DSVF 0");

        gerarComandos(se->entao, tabela, codigo);

        if (se->senao.empty()) {

            gerarSalto(codigo, saltoFalso, codigo.size());
            return;
        };

        const std::size_t saltoFim = codigo.size();
        codigo.push_back("DSVI 0");

        gerarSalto(codigo, saltoFalso, codigo.size());

        gerarComandos(se->senao, tabela, codigo);

        gerarSalto(codigo, saltoFim, codigo.size());
        return;
    };


    if (const auto* enquanto = dynamic_cast<const Enquanto*>(&comando)) {

        const std::size_t inicio = codigo.size();

        gerarCondicao(enquanto->condicao, tabela, codigo);

        const std::size_t saltoFim = codigo.size();
        codigo.push_back("DSVF 0");

        gerarComandos(enquanto->corpo, tabela, codigo);
        codigo.push_back("DSVI " + std::to_string(inicio));

        gerarSalto(codigo, saltoFim, codigo.size());
        return;
    }

    throw std::runtime_error("Comando não suportado!!");

};



void GeradorCodigo::gerarExpressao(const Expressao& expressao, const TabelaSimbolos& tabela, std::vector<std::string>& codigo) {

    if (const auto* numero = dynamic_cast<const Numero*>(&expressao)) {

        codigo.push_back("CRCT " + numero->valor);
        return;
    };


    if (const auto* identificador = dynamic_cast<const Identificador*>(&expressao)) {

        const int endereco = tabela.obter(identificador->id).endereco;
        codigo.push_back("CRVL " + std::to_string(endereco));
        return;
    };


    if (dynamic_cast<const Leitura*> (&expressao) != nullptr) {

        codigo.push_back("LEIT");
        return;
    };



    if (const auto* unaria = dynamic_cast<const ExpressaoUnaria*>(&expressao)) {

        gerarExpressao(*unaria->operando, tabela, codigo);
        codigo.push_back("INVE");
        return;
    };




    if (const auto* binaria = dynamic_cast<const ExpressaoBinaria*>(&expressao)) {

        gerarExpressao(*binaria->esquerda, tabela, codigo);
        gerarExpressao(*binaria->direita, tabela, codigo);

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



void GeradorCodigo::gerarCondicao(const Condicao& condicao, const TabelaSimbolos& tabela, std::vector<std::string>& codigo) {

    gerarExpressao(*condicao.esquerda, tabela, codigo);
    gerarExpressao(*condicao.direita, tabela, codigo);

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





void GeradorCodigo::gerarSalto(std::vector<std::string>& codigo, std::size_t indice, std::size_t destino) {

    const std::string instrucao = codigo.at(indice).substr(0, codigo.at(indice).find(' '));
    codigo.at(indice) = instrucao + " " + std::to_string(destino);
}