# Compilador LALG-C++ restrito e Máquina Virtual MaqHipo

Este repositório reúne uma máquina virtual MaqHipo e um compilador em
construção para a linguagem definida em `contexto/lalg-cpp.txt`. A linguagem
é inspirada em C++, mas aceita somente as produções dessa gramática.

## Estado do projeto

- Máquina virtual: implementada e testada.
- Analisador léxico: implementado e validado com `contexto/correto.cpp.txt`.
- Compilador: análise léxica, sintática, semântica e geração de código objeto
  para a MaqHipo implementadas.

O roteiro técnico e os requisitos revisados estão em
[`PLANEJAMENTO.md`](PLANEJAMENTO.md).
O desenho e as decisões do parser estão em
[`docs/analise-sintatica.md`](docs/analise-sintatica.md).

## Compilar e executar

Na raiz do projeto:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic \
    compilador/main.cpp \
    compilador/Lexico.cpp \
    compilador/Sintatico.cpp \
    compilador/AST.cpp \
    compilador/ConversorAST.cpp \
    compilador/TabelaSimbolos.cpp \
    compilador/Semantico.cpp \
    compilador/GeradorCodigo.cpp \
    -o lalgc

g++ -std=c++17 -Wall -Wextra -pedantic \
    maquina/main.cpp \
    maquina/Maquina.cpp \
    maquina/ConversorOperacao.cpp \
    maquina/PilhaDados.cpp \
    -o maquina_virtual
```

Compile um programa-fonte e gere o arquivo objeto:

```bash
./lalgc contexto/minimo.cpp.txt minimo.obj
```

Execute o arquivo objeto na MaqHipo:

```bash
./maquina_virtual minimo.obj
```

Para o programa mínimo, a saída esperada é `2` e `3` em linhas separadas.

## Linguagem-fonte

Consulte `contexto/lalg-cpp.txt` para a gramática completa. Alguns limites
importantes da linguagem são:

- somente variáveis `double`;
- declaração sem inicialização, seguida de atribuição quando necessária:

  ```cpp
  double valor;
  valor = 5.0;
  ```

- leitura por `lerDouble()`;
- saída no formato `cout << identificador << endl;`;
- números escritos com um dígito inicial, como `0.5` em vez de `.5`.

## Estrutura

```text
compilador/       analisador léxico e, futuramente, sintático e semântico
maquina/          interpretador da MaqHipo
codigo-objeto/    exemplos de programas objeto
contexto/         enunciado, gramática, exemplo-fonte e slides da disciplina
```

## Uso de IA

O desenvolvimento teve assistência do OpenAI Codex para explicações,
revisões de código e apoio à documentação. O entendimento, a implementação e
a apresentação do trabalho permanecem sob responsabilidade do aluno.
