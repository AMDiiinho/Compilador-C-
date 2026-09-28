#pragma once

#include <filesystem>
#include <string>

struct No;
struct Programa;

// Cria os registros visuais de uma compilacao aceita pelo analisador sintatico.
class GeradorArtefatos {
public:
    std::filesystem::path gerar(const std::string& caminhoFonte,
                                const std::string& nomePrograma,
                                const No& arvoreSintatica,
                                const Programa& ast) const;
};
