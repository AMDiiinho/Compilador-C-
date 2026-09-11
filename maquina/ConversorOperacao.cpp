#include "ConversorOperacaoInterface.hpp"

#include <stdexcept>

//convertendo a string operacao lida para a classe Operacao const std::string& op Lé
Operacao converterOperacao(const std::string& op) {
    if (op == "CRCT") return Operacao::CRCT;
    if (op == "CRVL") return Operacao::CRVL;
    if (op == "SOMA") return Operacao::SOMA;
    if (op == "SUBT") return Operacao::SUBT;
    if (op == "MULT") return Operacao::MULT;
    if (op == "DIVI") return Operacao::DIVI;
    if (op == "INVE") return Operacao::INVE;
    if (op == "CONJ") return Operacao::CONJ;
    if (op == "DISJ") return Operacao::DISJ;
    if (op == "NEGA") return Operacao::NEGA;
    if (op == "CPME") return Operacao::CPME;
    if (op == "CPMA") return Operacao::CPMA;
    if (op == "CPIG") return Operacao::CPIG;
    if (op == "CDES") return Operacao::CDES;
    if (op == "CPMI") return Operacao::CPMI;
    if (op == "CMAI") return Operacao::CMAI;
    if (op == "ARMZ") return Operacao::ARMZ;
    if (op == "DSVI") return Operacao::DSVI;
    if (op == "DSVF") return Operacao::DSVF;
    if (op == "LEIT") return Operacao::LEIT;
    if (op == "IMPR") return Operacao::IMPR;
    if (op == "ALME") return Operacao::ALME;
    if (op == "INPP") return Operacao::INPP;
    if (op == "PARA") return Operacao::PARA;

    throw std::runtime_error("Instrução desconhecida: " + op);
}
