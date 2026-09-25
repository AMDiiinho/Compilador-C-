# Planejamento — Compilador para MaqHipo

## Situação em 21/09/2026

A parte 2, a máquina virtual, está implementada em `maquina/`. Ela executa
arquivos objeto da MaqHipo, com uma instrução por linha.

O analisador léxico está implementado em `compilador/` e foi validado com
`programas/correto.cpp.txt`. Ele reconhece todas as palavras, operadores e
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
| `programas/lalg-cpp.txt` | A linguagem-fonte restrita que o compilador deve aceitar. |
| `programas/correto.cpp.txt` | Programa completo de referência para a validação final. |
| `artefacts/first-follow-sintatico.xlsx` | Conjuntos FIRST e FOLLOW usados pelo parser. |
| `artefacts/tabela-preditiva-sintatico.xlsx` | Tabela preditiva LL(1). |
| `artefacts/descendente não-recursivo manual.xlsx` | Material de apoio para a técnica do parser. |

## Linguagem aceita

O projeto aceita somente a linguagem descrita em `programas/lalg-cpp.txt`, e
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
  maquina/PilhaDados.cpp -o exec/maquina_virtual

./exec/maquina_virtual codigo-objeto/minimo.obj
```

### Compilador e geração de código

Arquivos principais:

- `compilador/TokenInterface.hpp`: tipos de token e estrutura `Token`;
- `compilador/LexicoInterface.hpp`: interface do analisador;
- `compilador/Lexico.cpp`: reconhecimento dos tokens;
- `compilador/Semantico.cpp`: valida declarações e usos de identificadores;
- `compilador/GeradorCodigo.cpp`: traduz a AST para instruções da MaqHipo;
- `compilador/main.cpp`: recebe fonte e destino `.obj` na linha de comando.

Fluxo validado:

```bash
mkdir -p exec

g++ -std=c++17 -Wall -Wextra -pedantic \
  compilador/main.cpp compilador/Lexico.cpp compilador/Sintatico.cpp \
  compilador/AST.cpp compilador/ConversorAST.cpp \
  compilador/TabelaSimbolos.cpp compilador/Semantico.cpp \
  compilador/GeradorCodigo.cpp -o exec/compilador
./exec/compilador programas/minimo.cpp.txt minimo.obj
./exec/maquina_virtual codigo-objeto/minimo.obj
```

O compilador gera `INPP`, `ALME`, instruções para expressões, leitura, saída,
condições, saltos e `PARA`. A MaqHipo executou o objeto do programa mínimo e
do programa completo.

## Analisador sintático e AST — concluído

Os conjuntos já calculados estão em `artefacts/first-follow-sintatico.xlsx` e
a tabela resultante está em `artefacts/tabela-preditiva-sintatico.xlsx`.

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
`programas/minimo.cpp.txt` foi mantido compatível com a gramática (declaração e
atribuição são produções distintas; não há `return`).

Validação local:

```bash
./exec/compilador programas/minimo.cpp.txt minimo.obj
./exec/maquina_virtual codigo-objeto/minimo.obj
```

## Etapa atual e entrega

1. **Validação manual:** executar casos válidos e inválidos durante cada etapa,
   conferindo a AST, a precedência/associatividade e os diagnósticos emitidos.
2. **Entrega:** documentar compilação e uso, incluir exemplos manuais válidos e
   inválidos e declarar o uso de IA, como exige o enunciado.

## Fluxo final

```text
fonte restrito -> léxico -> sintático -> semântico + geração -> programa.obj
                                                               -> MaqHipo
```
