#pragma once

// Operações da MaqHipo
enum class Operacao {
    CRCT, CRVL, SOMA, SUBT, MULT, DIVI, INVE, CONJ,
    CPME, DISJ, NEGA, CPMA, CPIG, CDES, CPMI, CMAI,
    ARMZ, DSVI, DSVF, LEIT, IMPR, ALME, INPP, PARA
};

struct Instrucao {
    Operacao operacao;       // Operação que a máquina vai executar
    double arg = 0.0;  // Valor da operação, se houver
};
