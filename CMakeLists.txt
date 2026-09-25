cmake_minimum_required(VERSION 3.16)

project(CompiladorLALGCPP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/exec")

add_executable(compilador
    compilador/main.cpp
    compilador/Lexico.cpp
    compilador/Sintatico.cpp
    compilador/AST.cpp
    compilador/ConversorAST.cpp
    compilador/TabelaSimbolos.cpp
    compilador/Semantico.cpp
    compilador/GeradorCodigo.cpp
)

target_compile_options(compilador PRIVATE
    -Wall
    -Wextra
    -pedantic
)

add_executable(maquina_virtual
    maquina/main.cpp
    maquina/Maquina.cpp
    maquina/ConversorOperacao.cpp
    maquina/PilhaDados.cpp
)

target_compile_options(maquina_virtual PRIVATE
    -Wall
    -Wextra
    -pedantic
)
