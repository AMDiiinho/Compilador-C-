# Planejamento — Compilador para MaqHipo

## Situação em 21/09/2026

A parte 2, a máquina virtual, está implementada em `maquina/`. Ela executa
arquivos objeto da MaqHipo, com uma instrução por linha.

O analisador léxico está implementado em `compilador/` e foi validado com
`contexto/correto.cpp.txt`. Ele reconhece todas as palavras, operadores e
delimitadores usados pela gramática, mantém linha e coluna, e termina com o
token `FIMARQUIVO`.

O analisador sintático está implementado. Ele usa análise **descendente não
recursiva**, isto é, um parser preditivo LL(1) dirigido por tabela e por uma
pilha explícita. Ao reconhecer a entrada, constrói uma árvore sintática
concreta e a converte para uma AST que preserva comandos, condições e
expressões.

## Materiais revisados

| Material | O que define |
| --- | --- |
| `contexto/enunciado.txt` | Compilador com análise léxica, sintática e semântica; geração de código objeto; proibição de Lex/Flex e semelhantes. |
| `contexto/lalg-cpp.txt` | A linguagem-fonte restrita que o compilador deve aceitar. |
| `contexto/correto.cpp.txt` | Programa completo de referência para a validação final. |
| `contexto/Aula04.odp` a `Aula06.odp` | FIRST, FOLLOW e análise descendente preditiva, inclusive sem recursão. |
| `contexto/Aula10.odp` | Tabela de símbolos e ações semânticas. |
| `contexto/Aula13.odp` | Instruções e modelo de pilha da MaqHipo. |

## Linguagem aceita

O projeto aceita somente a linguagem descrita em `contexto/lalg-cpp.txt`, e
não C++ completo. Em especial:

- `double` é o único tipo de variável;
- declarações e atribuições são construções separadas: `double x; x = 5.0;`;
- a entrada usa `lerDouble()` e a saída usa `cout << id << endl;`;
- expressões têm `+`, `-`, `*`, `/`, parênteses e menos unário;
- condicionais usam uma relação (`==`, `!=`, `>=`, `<=`, `>`, `<`);
- números válidos começam por dígito, como `10`, `5.0` e `0.5`.

## Componentes concluídos

### Máquina virtual

Compilação e execução validadas:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic \
  maquina/main.cpp maquina/Maquina.cpp maquina/ConversorOperacao.cpp \
  maquina/PilhaDados.cpp -o maquina_virtual

./maquina_virtual codigo-objeto/exemploSOMA.obj
```

### Analisador léxico

Arquivos principais:

- `compilador/TokenInterface.hpp`: tipos de token e estrutura `Token`;
- `compilador/LexicoInterface.hpp`: interface do analisador;
- `compilador/Lexico.cpp`: reconhecimento dos tokens;
- `compilador/testeLexico.cpp`: programa temporário de inspeção.

Teste executado:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic \
  compilador/testeLexico.cpp compilador/Lexico.cpp -o lexer
./lexer
```

O executável `lexer` é local e está ignorado pelo Git.

## Analisador sintático e AST — concluído

Os conjuntos já calculados estão em `docs/first-follow-sintatico.md` e a
tabela resultante está em `docs/tabela-preditiva-sintatico.md`.

1. Transcrever a gramática para símbolos internos: terminais correspondem a
   `TipoToken`; não terminais representam `PROG`, `CMDS`, `EXPRESSAO` e os
   demais nomes de `lalg-cpp.txt`.
2. Calcular e registrar os conjuntos FIRST e FOLLOW de cada não terminal.
3. Construir a tabela preditiva `M[naoTerminal][token]`. A ausência de uma
   produção na célula é erro sintático.
4. Criar uma pilha que inicia com `FIMARQUIVO` e `PROG`. Para cada topo:
   - terminal igual ao token atual: desempilhar e ler o próximo token;
   - terminal diferente: registrar erro;
   - não terminal: consultar `M` e empilhar a produção escolhida em ordem
     inversa.
5. Aceitar apenas quando a pilha e a entrada terminarem em `FIMARQUIVO`.

O parser reconhece programas válidos e rejeita os inválidos com posição do
token. A conversão para AST cobre declaração, atribuição, saída, `if`/`else`,
`while`, condições relacionais, leitura e expressões aritméticas. O exemplo
`contexto/minimo.cpp.txt` foi mantido compatível com a gramática (declaração e
atribuição são produções distintas; não há `return`).

Validação local:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic \
  compilador/testeSintatico.cpp compilador/Lexico.cpp \
  compilador/Sintatico.cpp compilador/AST.cpp compilador/ConversorAST.cpp \
  -o sintatico
./sintatico contexto/minimo.cpp.txt
./sintatico contexto/correto.cpp.txt
```

## Etapas posteriores

1. **Validação manual:** executar casos válidos e inválidos durante cada etapa,
   conferindo a AST, a precedência/associatividade e os diagnósticos emitidos.
2. **Semântica:** tabela `nome -> {endereço, tipo}`, detecção de uso antes de
   declaração e redeclaração.
3. **Expressões e código:** emitir `CRCT`, `CRVL`, `SOMA`, `SUBT`, `MULT`,
   `DIVI`, `INVE`, `ARMZ`, `LEIT` e `IMPR`.
4. **Controle de fluxo:** gerar `DSVF` e `DSVI` com backpatching para `if` e
   `while`.
5. **Integração:** todo programa objeto começa com `INPP` e termina com
   `PARA`; o compilador só grava o `.obj` definitivo se não houver erros.
6. **Entrega:** documentar compilação e uso, incluir exemplos manuais válidos e
   inválidos e declarar o uso de IA, como exige o enunciado.

## Fluxo final

```text
fonte restrito -> léxico -> sintático -> semântico + geração -> programa.obj
                                                               -> MaqHipo
```
