# Compilador LALG-C++ restrito e Máquina Virtual MaqHipo

Este repositório reúne uma máquina virtual MaqHipo e um compilador em
construção para a linguagem definida em `contexto/lalg-cpp.txt`. A linguagem
é inspirada em C++, mas aceita somente as produções dessa gramática.

## Estado do projeto

- Máquina virtual: implementada e testada.
- Analisador léxico: implementado e validado com `contexto/correto.cpp.txt`.
- Analisador sintático preditivo não recursivo: próxima etapa.
- Análise semântica e geração de código objeto: pendentes.

O roteiro técnico e os requisitos revisados estão em
[`PLANEJAMENTO.md`](PLANEJAMENTO.md).
Os conjuntos usados para construir o parser estão em
[`docs/first-follow-sintatico.md`](docs/first-follow-sintatico.md).
A tabela preditiva resultante está em
[`docs/tabela-preditiva-sintatico.md`](docs/tabela-preditiva-sintatico.md).

## Compilar e executar a máquina virtual

Na raiz do projeto:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic \
    maquina/main.cpp \
    maquina/Maquina.cpp \
    maquina/ConversorOperacao.cpp \
    maquina/PilhaDados.cpp \
    -o maquina_virtual
```

Execute um arquivo `.obj`:

```bash
./maquina_virtual codigo-objeto/exemploSOMA.obj
```

Outros exemplos disponíveis:

```bash
./maquina_virtual codigo-objeto/exemploSALTOparaPARA.obj
./maquina_virtual codigo-objeto/exemplo.obj
```

## Testar o analisador léxico

O teste atual lê `contexto/correto.cpp.txt` e imprime uma lista compacta dos
lexemas reconhecidos; `$` representa o token `FIMARQUIVO`. Linha e coluna são
mantidas internamente em cada token para diagnósticos futuros:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic \
    compilador/testeLexico.cpp compilador/Lexico.cpp \
    -o lexer

./lexer
```

O binário `lexer` é ignorado pelo Git.

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
