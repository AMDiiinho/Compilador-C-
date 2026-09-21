#pragma once

#include "ASTInterface.hpp"
#include "ArvoreSintaticaInterface.hpp"

// Convertendo a árvore sintática concreta produzida pelo sintatico em uma AST abstrata, que será consumida pelo semantico

class ConversorAST {

    public:

        // Receve a raíz da árvore concreta, que deve ser PROG e retorna a raiz da AST
        Programa converter(const No& raiz) const;

    private:

        // Dado um nó e a posição de um filho, retorno esse filho
        // o indice indica qual filho acessaremos
        // const No& no, recebe um nó da árvore por referência que essa função não pode alterar
        // No&, faz com que o retorno também seja por referência, ao invés de retornar uma cópia do filho, devolve uma referência ao filho que está
        // na árvore
        // Static indica que o método filho pertence à classe ConversorAST, mas não precisa de uma instancia de ConversorAST para ser chamada
        static const No& filho(const No& no, std::size_t indice);

        // Dado um Nó, retorno o Token por referência deste nó
        static const Token& tokenNo(const No& no);

        //dado um no e um naoTerminal, retorno se é ou não é naoTerminal
        static bool ehNaoTerminal(const No& no, NaoTerminal naoTerminal);

        // métodos de conversão de concreto para abstrado, Ex: IDENTIFICADOR --> Declaração: cont
        ExpressaoPtr converterExpId(const No& expId) const;
        ExpressaoPtr converterExpressao(const No& exp) const;
        ExpressaoPtr converterTermo(const No& termo) const;
        ExpressaoPtr converterFator(const No& fator) const;

        ExpressaoPtr converterOutrosTermos(const No& outrosTermos, ExpressaoPtr esquerda) const;
        ExpressaoPtr converterMaisFatores(const No& maisFatores, ExpressaoPtr esquerda) const;

        // metodos de conversão para condições Ex: IF -> (COND) { CMDS }; IF -> Se cond; então Comandos; senao Comandos
        Condicao converterCondicao(const No& condicao) const;


        //demais meétodos de conversão
        static OperadorBinario converterOperadorBinario(TipoToken tipo);
        static OperadorRelacional converterOperacaoRelacional(TipoToken tipo);

        ComandoPtr converterComando(const No& cmd) const;
        ComandoPtr converterDeclaracao(const No& var) const;

        static void coletarIds(const No& vars, std::vector<std::string>& ids);

        ComandoPtr converterComandoCondicional(const No& cmdCond) const;

        std::vector<ComandoPtr> converterComandos(const No& cmds) const;
        std::vector<ComandoPtr> converterDc(const No& dc) const;
        std::vector<ComandoPtr> converterMaisComandos(const No& maisCmds) const;

        static void adicionarComandos(std::vector<ComandoPtr>& destino, std::vector<ComandoPtr> origem);

};