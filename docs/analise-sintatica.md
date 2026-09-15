# Analisador sintático LL(1) com tabela M e AST

## Resumo

O lexer continuará produzindo `Token { tipo, lexema, linha, coluna }`. O parser
consumirá tokens sob demanda com `proximoToken()`, mantendo apenas um token de
*lookahead*; a impressão como lista será somente diagnóstico do programa de
teste.

## Mudanças principais

- Manter `linha` e `coluna` em `Token` e `Lexico` para diagnósticos; não exibi-las
  no teste léxico.
- Atualizar o teste léxico para imprimir uma lista compacta, por exemplo
  `{#include, <, iostream, ..., $}`, onde `$` representa `FIMARQUIVO`.
- Manter `Token Lexico::proximoToken()` como contrato entre lexer e parser; não
  criar `vector<Token>` como API principal.
- Criar `Sintatico(Lexico&)` com `Programa analisar()`. O parser usará pilha
  explícita iniciada por `FIMARQUIVO` e `PROG`.
- Representar a tabela M como matriz imutável de `NaoTerminal × TipoToken`, cujas
  células referenciam um inventário único de produções. Células vazias sinalizam
  erro.
- Mapear `<` e `>` da tabela para `TipoToken::MENOR` e `TipoToken::MAIOR`; `<<`
  usa `DESLOCAESQUERDA`.
- Construir uma árvore concreta interna durante o reconhecimento e convertê-la,
  após sucesso, em AST abstrata:
  - comandos de declaração, atribuição, saída, `if`/`else` e `while`;
  - condições relacionais;
  - expressões de identificador, número, `lerDouble()`, menos unário e
    operadores aritméticos.
- Não armazenar posição de origem na AST. `ErroSintatico` informará o lexema/tipo
  encontrado, linha, coluna e o conjunto de tokens esperados, interrompendo na
  primeira falha.

## Documentação e validação

- Corrigir FIRST/FOLLOW: `EXP_IDENT`, `CONDICAO`, `EXPRESSAO` e `TERMO` não
  começam com `ε` nem `+`; `FOLLOW(OP_AD)` é `{-, id, numero_real, (}`.
- Atualizar README e o planejamento para documentar o formato compacto do teste
  léxico e a preservação interna de linha/coluna.
- Criar testes com fontes rastreadas para programas válidos, forma da AST,
  precedência/associatividade e erros como `;` ausente, expressão incompleta,
  relação ausente, `else` isolado, delimitadores incorretos e entrada após `}`.

## Premissas

- A primeira versão para no primeiro erro sintático.
- O reconhecimento permanece LL(1), dirigido por tabela e não recursivo.
- Sem tabela de símbolos, semântica ou geração de código nesta etapa.
