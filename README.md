# Máquina Virtual MaqHipo

Implementação em C++ de uma máquina virtual que interpreta arquivos de código objeto (`.obj`).

## Compilação

Na raiz do projeto, execute:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic \
    maquina/main.cpp \
    maquina/Maquina.cpp \
    maquina/ConversorOperacao.cpp \
    maquina/PilhaDados.cpp \
    -o maquina_virtual
```

## Execução

Informe o caminho de um arquivo `.obj` ao executar o programa:

```bash
./maquina_virtual codigo-objeto/exemploSOMA.obj
```

Outros exemplos disponíveis:

```bash
./maquina_virtual codigo-objeto/exemploSALTOparaPARA.obj
./maquina_virtual codigo-objeto/exemplo.obj
```

Se nenhum arquivo for informado, o programa mostra a forma correta de uso.
