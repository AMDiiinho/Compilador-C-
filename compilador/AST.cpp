#include "ASTInterface.hpp"

#include <ostream>
#include <stdexcept>
#include <string>

namespace {

    
// Imprime uma linha da árvore usando o prefixo que representa a posição do nó.
// O parâmetro ultimo define qual ramo deve ser desenhado.
void imprimirLinha(std::ostream& saida, const std::string& prefixo, bool ultimo, const std::string& texto) {
    
    saida << prefixo << (ultimo ? "└── " : "├── ") << texto << '\n';
}





// Monta o prefixo que será usado pelos filhos do nó atual.
// Quando o nó é o último irmão, não é necessário prolongar a linha vertical.
std::string prefixoFilhos(const std::string& prefixo, bool ultimo) {
    
    return prefixo + (ultimo ? "    " : "│   ");
}





// Converte o operador binário da AST para o texto usado na impressão.
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





// Converte o operador relacional da AST para o texto usado na impressão.
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





// Imprime uma expressão identificando o tipo concreto do nó e seus filhos.
// A expressão pode ser um valor, uma leitura ou uma operação.
void imprimirExpressao(const Expressao& expressao, std::ostream& saida, const std::string& prefixo, bool ultimo) {
    
    // Identificador representa o uso de uma variável na expressão.
    if (const auto* identificador = dynamic_cast<const Identificador*>(&expressao)) {
        
        imprimirLinha(saida, prefixo, ultimo, "Identificador: " + identificador->id);
        return;
    }

    // Numero representa um valor constante.
    if (const auto* numero = dynamic_cast<const Numero*>(&expressao)) {
        
        imprimirLinha(saida, prefixo, ultimo, "Numero: " + numero->valor);
        return;
    }

    // Leitura representa a chamada da operação lerDouble().
    if (dynamic_cast<const Leitura*>(&expressao) != nullptr) {
        
        imprimirLinha(saida, prefixo, ultimo, "Leitura: lerDouble()");
        return;
    }

    // Uma expressão unária possui apenas um filho, que é o seu operando.
    if (const auto* unaria = dynamic_cast<const ExpressaoUnaria*>(&expressao)) {
        
        imprimirLinha(saida, prefixo, ultimo, "Menos unario");
        imprimirExpressao(*unaria->operando, saida, prefixoFilhos(prefixo, ultimo), true);
        
        return;
    }

    // Uma expressão binária possui a expressão da esquerda e a da direita.
    if (const auto* binaria = dynamic_cast<const ExpressaoBinaria*>(&expressao)) {
        
        imprimirLinha(saida, prefixo, ultimo,"Operacao: " + nomeOperadorBinario(binaria->operador));
        const std::string filhos = prefixoFilhos(prefixo, ultimo);
        imprimirExpressao(*binaria->esquerda, saida, filhos, false);
        imprimirExpressao(*binaria->direita, saida, filhos, true);
        
        return;
    }

    throw std::logic_error("Tipo de expressao desconhecido.");
}





// Imprime o operador relacional e as duas expressões comparadas.
void imprimirCondicao(const Condicao& condicao, std::ostream& saida, const std::string& prefixo, bool ultimo) {
    
    imprimirLinha(saida, prefixo, ultimo, "Condicao: " + nomeOperadorRelacional(condicao.operador));
    const std::string filhos = prefixoFilhos(prefixo, ultimo);
    imprimirExpressao(*condicao.esquerda, saida, filhos, false);
    imprimirExpressao(*condicao.direita, saida, filhos, true);
}





// Declaração antecipada usada porque imprimirComandos chama imprimirComando.
void imprimirComando(const Comando& comando, std::ostream& saida, const std::string& prefixo, bool ultimo);





// Imprime os comandos na ordem em que aparecem no programa.
// O último comando precisa ser identificado para desenhar os ramos corretamente.
void imprimirComandos(const std::vector<ComandoPtr>& comandos, std::ostream& saida, const std::string& prefixo) {
    
    for (std::size_t indice = 0; indice < comandos.size(); ++indice) {
        const bool ultimo = indice + 1 == comandos.size();
        imprimirComando(*comandos[indice], saida, prefixo, ultimo);
    }
}





// Imprime o nome de um bloco e todos os comandos que pertencem a ele.
void imprimirBloco(const std::string& nome, const std::vector<ComandoPtr>& comandos, std::ostream& saida, const std::string& prefixo, bool ultimo) {
    
    imprimirLinha(saida, prefixo, ultimo, nome);
    imprimirComandos(comandos, saida, prefixoFilhos(prefixo, ultimo));
}






// Imprime um comando da AST de acordo com seu tipo concreto.
// Comandos compostos também imprimem suas condições e seus blocos recursivamente.
void imprimirComando(const Comando& comando, std::ostream& saida, const std::string& prefixo, bool ultimo) {
    
    // Uma declaração pode conter vários identificadores no mesmo comando.
    if (const auto* declaracao = dynamic_cast<const Declaracao*>(&comando)) {
        
        std::string texto = "Declaracao:";
        
        for (const std::string& id : declaracao->ids) {
            texto += " " + id;
        }

        imprimirLinha(saida, prefixo, ultimo, texto);
        return;
    }

    // A atribuição possui o destino e a expressão cujo valor será armazenado.
    if (const auto* atribuicao = dynamic_cast<const Atribuicao*>(&comando)) {
        imprimirLinha(saida, prefixo, ultimo, "Atribuicao: " + atribuicao->destino);
        imprimirExpressao(*atribuicao->expressao, saida, prefixoFilhos(prefixo, ultimo), true);
        
        return;
    }

    // Print guarda o identificador cujo valor será enviado para a saída.
    if (const auto* print = dynamic_cast<const Print*>(&comando)) {
        imprimirLinha(saida, prefixo, ultimo, "Print: " + print->id);
        
        return;
    }

    // Se possui uma condição, um bloco Entao e, opcionalmente, um bloco Senao.
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

    // Enquanto possui uma condição e um bloco Corpo que pode ser repetido.
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

// Imprime a raiz da AST e inicia a impressão dos comandos do programa.
void imprimirAST(const Programa& programa, std::ostream& saida) {
    saida << "Programa\n";
    imprimirComandos(programa.comandos, saida, "");
}
