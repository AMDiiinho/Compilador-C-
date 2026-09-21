#include "ASTInterface.hpp"

#include <ostream>
#include <stdexcept>
#include <string>

namespace {

    
void imprimirLinha(std::ostream& saida, const std::string& prefixo, bool ultimo, const std::string& texto) {
    
    saida << prefixo << (ultimo ? "└── " : "├── ") << texto << '\n';
}





std::string prefixoFilhos(const std::string& prefixo, bool ultimo) {
    
    return prefixo + (ultimo ? "    " : "│   ");
}





std::string nomeOperadorBinario(OperadorBinario operador) {
    switch (operador) {
        
        case OperadorBinario::Soma: 
            return "+";
        
        case OperadorBinario::Subtracao: 
            return "-";
        
        case OperadorBinario::Multiplicacao: 
            return "*";
        
        case OperadorBinario::Divisao: 
            return "/";
    }

    throw std::logic_error("Operador binario desconhecido.");
}





std::string nomeOperadorRelacional(OperadorRelacional operador) {
    switch (operador) {
        
        case OperadorRelacional::Igual: 
            return "==";
        
        case OperadorRelacional::Diferente: 
            return "!=";
        
        case OperadorRelacional::Maior: 
            return ">";
        
        case OperadorRelacional::Menor: 
            return "<";
        
        case OperadorRelacional::MaiorIgual: 
            return ">=";
        
        case OperadorRelacional::MenorIgual: 
            return "<=";
    }

    throw std::logic_error("Operador relacional desconhecido.");
}





void imprimirExpressao(const Expressao& expressao, std::ostream& saida, const std::string& prefixo, bool ultimo) {
    
    if (const auto* identificador = dynamic_cast<const Identificador*>(&expressao)) {
        
        imprimirLinha(saida, prefixo, ultimo, "Identificador: " + identificador->id);
        return;
    }

    if (const auto* numero = dynamic_cast<const Numero*>(&expressao)) {
        
        imprimirLinha(saida, prefixo, ultimo, "Numero: " + numero->valor);
        return;
    }

    if (dynamic_cast<const Leitura*>(&expressao) != nullptr) {
        
        imprimirLinha(saida, prefixo, ultimo, "Leitura: lerDouble()");
        return;
    }

    if (const auto* unaria = dynamic_cast<const ExpressaoUnaria*>(&expressao)) {
        
        imprimirLinha(saida, prefixo, ultimo, "Menos unario");
        imprimirExpressao(*unaria->operando, saida, prefixoFilhos(prefixo, ultimo), true);
        
        return;
    }

    if (const auto* binaria = dynamic_cast<const ExpressaoBinaria*>(&expressao)) {
        
        imprimirLinha(saida, prefixo, ultimo,"Operacao: " + nomeOperadorBinario(binaria->operador));
        const std::string filhos = prefixoFilhos(prefixo, ultimo);
        imprimirExpressao(*binaria->esquerda, saida, filhos, false);
        imprimirExpressao(*binaria->direita, saida, filhos, true);
        
        return;
    }

    throw std::logic_error("Tipo de expressao desconhecido.");
}





void imprimirCondicao(const Condicao& condicao, std::ostream& saida, const std::string& prefixo, bool ultimo) {
    
    imprimirLinha(saida, prefixo, ultimo, "Condicao: " + nomeOperadorRelacional(condicao.operador));
    const std::string filhos = prefixoFilhos(prefixo, ultimo);
    imprimirExpressao(*condicao.esquerda, saida, filhos, false);
    imprimirExpressao(*condicao.direita, saida, filhos, true);
}





void imprimirComando(const Comando& comando, std::ostream& saida, const std::string& prefixo, bool ultimo);





void imprimirComandos(const std::vector<ComandoPtr>& comandos, std::ostream& saida, const std::string& prefixo) {
    
    for (std::size_t indice = 0; indice < comandos.size(); ++indice) {
        const bool ultimo = indice + 1 == comandos.size();
        imprimirComando(*comandos[indice], saida, prefixo, ultimo);
    }
}





void imprimirBloco(const std::string& nome, const std::vector<ComandoPtr>& comandos, std::ostream& saida, const std::string& prefixo, bool ultimo) {
    
    imprimirLinha(saida, prefixo, ultimo, nome);
    imprimirComandos(comandos, saida, prefixoFilhos(prefixo, ultimo));
}






void imprimirComando(const Comando& comando, std::ostream& saida, const std::string& prefixo, bool ultimo) {
    
    if (const auto* declaracao = dynamic_cast<const Declaracao*>(&comando)) {
        
        std::string texto = "Declaracao:";
        
        for (const std::string& id : declaracao->ids) {
            texto += " " + id;
        }

        imprimirLinha(saida, prefixo, ultimo, texto);
        return;
    }

    if (const auto* atribuicao = dynamic_cast<const Atribuicao*>(&comando)) {
        imprimirLinha(saida, prefixo, ultimo, "Atribuicao: " + atribuicao->destino);
        imprimirExpressao(*atribuicao->expressao, saida, prefixoFilhos(prefixo, ultimo), true);
        
        return;
    }

    if (const auto* print = dynamic_cast<const Print*>(&comando)) {
        imprimirLinha(saida, prefixo, ultimo, "Print: " + print->id);
        
        return;
    }

    if (const auto* se = dynamic_cast<const Se*>(&comando)) {
        
        imprimirLinha(saida, prefixo, ultimo, "Se");
        const std::string filhos = prefixoFilhos(prefixo, ultimo);
        const bool temSenao = !se->senao.empty();
        imprimirCondicao(se->condicao, saida, filhos, false);
        imprimirBloco("Entao", se->entao, saida, filhos, !temSenao);
        
        if (temSenao) {
            imprimirBloco("Senao", se->senao, saida, filhos, true);
        }
        
        return;
    }

    if (const auto* enquanto = dynamic_cast<const Enquanto*>(&comando)) {
        
        imprimirLinha(saida, prefixo, ultimo, "Enquanto");
        const std::string filhos = prefixoFilhos(prefixo, ultimo);
        imprimirCondicao(enquanto->condicao, saida, filhos, false);
        imprimirBloco("Corpo", enquanto->corpo, saida, filhos, true);
        
        return;
    }

    throw std::logic_error("Tipo de comando desconhecido.");
}

} // namespace

void imprimirAST(const Programa& programa, std::ostream& saida) {
    saida << "Programa\n";
    imprimirComandos(programa.comandos, saida, "");
}
