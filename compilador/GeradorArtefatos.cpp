#include "GeradorArtefatosInterface.hpp"
#include "ASTInterface.hpp"
#include "ArvoreSintaticaInterface.hpp"
#include "LexicoInterface.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>
#include <vector>

namespace {
struct Node {
    std::string label;
    std::vector<Node> children;
    int x = 0;
    int y = 0;

    explicit Node(std::string label) : label(std::move(label)) {}
    Node(std::string label, std::vector<Node> children)
        : label(std::move(label)), children(std::move(children)) {}
};

std::string tipoToken(TipoToken t) {
    switch (t) {
        case TipoToken::INCLUDE: return "INCLUDE"; case TipoToken::IOSTREAM: return "IOSTREAM";
        case TipoToken::USING: return "USING"; case TipoToken::NAMESPACE: return "NAMESPACE";
        case TipoToken::STD: return "STD"; case TipoToken::INT: return "INT"; case TipoToken::MAIN: return "MAIN";
        case TipoToken::DOUBLE: return "DOUBLE"; case TipoToken::IF: return "IF"; case TipoToken::ELSE: return "ELSE";
        case TipoToken::WHILE: return "WHILE"; case TipoToken::COUT: return "COUT"; case TipoToken::ENDL: return "ENDL";
        case TipoToken::LERDOUBLE: return "LERDOUBLE"; case TipoToken::IDENTIFICADOR: return "IDENTIFICADOR";
        case TipoToken::NUMEROREAL: return "NUMEROREAL"; case TipoToken::ATRIBUICAO: return "ATRIBUICAO";
        case TipoToken::SOMA: return "SOMA"; case TipoToken::SUBTRACAO: return "SUBTRACAO";
        case TipoToken::MULTIPLICACAO: return "MULTIPLICACAO"; case TipoToken::DIVISAO: return "DIVISAO";
        case TipoToken::IGUAL: return "IGUAL"; case TipoToken::DIFERENTE: return "DIFERENTE"; case TipoToken::MAIOR: return "MAIOR";
        case TipoToken::MENOR: return "MENOR"; case TipoToken::MAIORIGUAL: return "MAIORIGUAL"; case TipoToken::MENORIGUAL: return "MENORIGUAL";
        case TipoToken::DESLOCAESQUERDA: return "DESLOCAESQUERDA"; case TipoToken::ABREPARENTESES: return "ABREPARENTESES";
        case TipoToken::FECHAPARENTESES: return "FECHAPARENTESES"; case TipoToken::ABRECHAVES: return "ABRECHAVES";
        case TipoToken::FECHACHAVES: return "FECHACHAVES"; case TipoToken::VIRGULA: return "VIRGULA";
        case TipoToken::PONTOEVIRGULA: return "PONTOEVIRGULA"; case TipoToken::FIMARQUIVO: return "FIMARQUIVO";
    } return "?";
}
std::string naoTerminal(NaoTerminal n) {
    static const char* nomes[] = {"PROG", "DC", "VAR", "VARS", "MAIS_VAR", "TIPO", "CMDS", "MAIS_CMDS", "CMD_COND", "CMD", "PFALSA", "RESTO_IDENT", "EXP_IDENT", "CONDICAO", "RELACAO", "EXPRESSAO", "TERMO", "OP_UN", "FATOR", "OUTROS_TERMOS", "OP_AD", "MAIS_FATORES", "OP_MUL"};
    return nomes[static_cast<int>(n)];
}
Node concreto(const No& no) {
    Node result(std::holds_alternative<TipoToken>(no.simbolo)
                    ? tipoToken(std::get<TipoToken>(no.simbolo))
                    : naoTerminal(std::get<NaoTerminal>(no.simbolo)));

    if (no.token && no.token->tipo != TipoToken::FIMARQUIVO) {
        result.label += ": " + no.token->lexema;
    }

    for (const auto& filho : no.filhos) {
        result.children.push_back(concreto(*filho));
    }

    // Um não terminal sem filhos representa uma produção vazia na tabela preditiva.
    if (std::holds_alternative<NaoTerminal>(no.simbolo) && no.filhos.empty()) {
        result.label += ": ε";
    }

    return result;
}
std::string binario(OperadorBinario op) { return op == OperadorBinario::Soma ? "+" : op == OperadorBinario::Subtracao ? "-" : op == OperadorBinario::Multiplicacao ? "*" : "/"; }
std::string relacional(OperadorRelacional op) { static const char* r[] = {"==", "!=", ">", "<", ">=", "<="}; return r[static_cast<int>(op)]; }
Node expressao(const Expressao& e) {
    if (auto p = dynamic_cast<const Identificador*>(&e)) {
        return Node("IDENTIFICADOR: " + p->id);
    }
    if (auto p = dynamic_cast<const Numero*>(&e)) {
        return Node("NUMERO: " + p->valor);
    }
    if (dynamic_cast<const Leitura*>(&e)) {
        return Node("LEITURA: lerDouble");
    }
    if (auto p = dynamic_cast<const ExpressaoUnaria*>(&e)) {
        return Node("MENOS UNARIO", {expressao(*p->operando)});
    }
    if (auto p = dynamic_cast<const ExpressaoBinaria*>(&e)) {
        return Node("OPERACAO: " + binario(p->operador),
                    {expressao(*p->esquerda), expressao(*p->direita)});
    }
    throw std::logic_error("Expressao AST desconhecida.");
}
Node condicao(const Condicao& c) {
    return Node("CONDICAO: " + relacional(c.operador),
                {expressao(*c.esquerda), expressao(*c.direita)});
}
Node comando(const Comando& c) {
    if (auto p = dynamic_cast<const Declaracao*>(&c)) {
        Node n("DECLARACAO");
        for (const auto& id : p->ids) n.children.emplace_back("IDENTIFICADOR: " + id);
        return n;
    }
    if (auto p = dynamic_cast<const Atribuicao*>(&c)) {
        return Node("ATRIBUICAO: " + p->destino, {expressao(*p->expressao)});
    }
    if (auto p = dynamic_cast<const Print*>(&c)) return Node("PRINT: " + p->id);
    if (auto p = dynamic_cast<const Se*>(&c)) {
        Node n("SE", {condicao(p->condicao)});
        Node entao("ENTAO");
        for (const auto& x : p->entao) entao.children.push_back(comando(*x));
        n.children.push_back(std::move(entao));
        if (!p->senao.empty()) {
            Node senao("SENAO");
            for (const auto& x : p->senao) senao.children.push_back(comando(*x));
            n.children.push_back(std::move(senao));
        }
        return n;
    }
    if (auto p = dynamic_cast<const Enquanto*>(&c)) {
        Node n("ENQUANTO", {condicao(p->condicao)});
        Node corpo("CORPO");
        for (const auto& x : p->corpo) corpo.children.push_back(comando(*x));
        n.children.push_back(std::move(corpo));
        return n;
    }
    throw std::logic_error("Comando AST desconhecido.");
}
Node abstrata(const Programa& p) {
    Node n("PROGRAMA");
    for (const auto& c : p.comandos) n.children.push_back(comando(*c));
    return n;
}
int layout(Node& n, int depth, int& leaf, int& maxDepth) {
    n.y = depth;
    maxDepth = std::max(maxDepth, depth);
    if (n.children.empty()) return n.x = leaf++;

    int first = -1;
    int last = -1;
    for (auto& child : n.children) {
        const int x = layout(child, depth + 1, leaf, maxDepth);
        if (first < 0) first = x;
        last = x;
    }
    return n.x = (first + last) / 2;
}

struct Canvas {
    int w;
    int h;
    std::vector<unsigned char> px;

    Canvas(int width, int height)
        : w(width), h(height), px(static_cast<size_t>(width) * height * 3, 255) {}

    void dot(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
        if (x < 0 || x >= w || y < 0 || y >= h) return;
        const auto index = (static_cast<size_t>(y) * w + x) * 3;
        px[index] = r;
        px[index + 1] = g;
        px[index + 2] = b;
    }
};

void line(Canvas& canvas, int x0, int y0, int x1, int y1) {
    int dx = std::abs(x1 - x0);
    const int sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0);
    const int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;

    while (true) {
        canvas.dot(x0, y0, 96, 112, 128);
        if (x0 == x1 && y0 == y1) break;
        const int twiceError = 2 * error;
        if (twiceError >= dy) { error += dy; x0 += sx; }
        if (twiceError <= dx) { error += dx; y0 += sy; }
    }
}
// Fonte bitmap 5x7: cada caractere e desenhado como um bloco legivel, inclusive em sistemas sem fontes instaladas.
const unsigned char* glyph(char c) { static const unsigned char letras[][7]={{14,17,17,31,17,17,17},{30,17,17,30,17,17,30},{15,16,16,16,16,16,15},{30,17,17,17,17,17,30},{31,16,16,30,16,16,31},{31,16,16,30,16,16,16},{15,16,16,23,17,17,15},{17,17,17,31,17,17,17},{31,4,4,4,4,4,31},{1,1,1,1,17,17,14},{17,18,20,24,20,18,17},{16,16,16,16,16,16,31},{17,27,21,21,17,17,17},{17,25,21,19,17,17,17},{14,17,17,17,17,17,14},{30,17,17,30,16,16,16},{14,17,17,17,21,18,13},{30,17,17,30,20,18,17},{15,16,16,14,1,1,30},{31,4,4,4,4,4,4},{17,17,17,17,17,17,14},{17,17,17,17,17,10,4},{17,17,17,21,21,21,10},{17,17,10,4,10,17,17},{17,17,10,4,4,4,4},{31,1,2,4,8,16,31}}; static const unsigned char digits[][7]={{14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},{30,1,1,14,1,1,30},{2,6,10,18,31,2,2},{31,16,30,1,1,17,14},{6,8,16,30,17,17,14},{31,1,2,4,8,8,8},{14,17,17,14,17,17,14},{14,17,17,15,1,2,12}}; c=static_cast<char>(std::toupper(static_cast<unsigned char>(c))); if(c>='A'&&c<='Z')return letras[c-'A'];if(c>='0'&&c<='9')return digits[c-'0']; static const unsigned char dash[7]={0,0,0,31,0,0,0}, colon[7]={0,4,0,0,4,0,0}, plus[7]={0,4,4,31,4,4,0}, star[7]={0,21,14,31,14,21,0}, slash[7]={1,2,4,8,16,0,0}, equal[7]={0,31,0,31,0,0,0}, space[7]={0,0,0,0,0,0,0}; switch(c){case '-':return dash;case ':':return colon;case '+':return plus;case '*':return star;case '/':return slash;case '=':return equal;default:return space;} }
std::string labelVisivel(const std::string& label) {
    constexpr std::size_t limite = 14;
    return label.size() <= limite ? label : label.substr(0, limite - 1) + "~";
}

void text(Canvas& c, int x, int y, const std::string& s) { for (char ch : s) { const auto* g = glyph(ch); for (int row = 0; row < 7; ++row) for (int col = 0; col < 5; ++col) if (g[row] & (1 << (4 - col))) for (int yy = 0; yy < 2; ++yy) for (int xx = 0; xx < 2; ++xx) c.dot(x + col * 2 + xx, y + row * 2 + yy, 25, 42, 58); x += 12; } }
void draw(Canvas& c, const Node& n) { const int x = n.x * 180 + 90; const int y = n.y * 74 + 32; for (const auto& child : n.children) { const int childX = child.x * 180 + 90; const int childY = child.y * 74 + 32; line(c, x, y + 16, childX, childY - 16); draw(c, child); } for (int yy = y - 16; yy <= y + 16; ++yy) for (int xx = x - 83; xx <= x + 83; ++xx) if (xx == x - 83 || xx == x + 83 || yy == y - 16 || yy == y + 16) c.dot(xx, yy, 35, 99, 140); text(c, x - 78, y - 7, labelVisivel(n.label)); }
uint32_t crc(const unsigned char* p,size_t n){uint32_t x=0xffffffff;for(size_t i=0;i<n;++i){x^=p[i];for(int j=0;j<8;++j)x=(x>>1)^(0xedb88320u&-(x&1));}return ~x;} void be(std::ofstream&o,uint32_t x){for(int s=24;s>=0;s-=8)o.put(static_cast<char>(x>>s));} void chunk(std::ofstream&o,const char* type,const std::vector<unsigned char>&d){be(o,d.size());o.write(type,4);if(!d.empty())o.write(reinterpret_cast<const char*>(d.data()),d.size());std::vector<unsigned char> c(type,type+4);c.insert(c.end(),d.begin(),d.end());be(o,crc(c.data(),c.size()));}
[[maybe_unused]] void png(const std::filesystem::path& path, Node root) { int leaves=0,depth=0;layout(root,0,leaves,depth); Canvas c(std::max(800,leaves*180),std::max(120,(depth+1)*74+50));draw(c,root);std::vector<unsigned char> raw;raw.reserve(static_cast<size_t>(c.h)*(c.w*3+1));for(int y=0;y<c.h;++y){raw.push_back(0);raw.insert(raw.end(),c.px.begin()+static_cast<size_t>(y)*c.w*3,c.px.begin()+static_cast<size_t>(y+1)*c.w*3);}std::vector<unsigned char> z={120,1};for(size_t off=0;off<raw.size();){size_t n=std::min<size_t>(65535,raw.size()-off);z.push_back(off+n==raw.size()?1:0);z.push_back(n&255);z.push_back(n>>8);auto nn=static_cast<unsigned short>(~n);z.push_back(nn&255);z.push_back(nn>>8);z.insert(z.end(),raw.begin()+off,raw.begin()+off+n);off+=n;}uint32_t a=1,b=0;for(auto v:raw){a=(a+v)%65521;b=(b+a)%65521;}for(int s=24;s>=0;s-=8)z.push_back(static_cast<unsigned char>(((b<<16)|a)>>s));std::ofstream o(path,std::ios::binary);if(!o)throw std::runtime_error("Nao foi possivel criar " + path.string());o.write("\x89PNG\r\n\x1a\n",8);std::vector<unsigned char> ihdr(13);ihdr[3]=c.w;ihdr[2]=c.w>>8;ihdr[1]=c.w>>16;ihdr[0]=c.w>>24;ihdr[7]=c.h;ihdr[6]=c.h>>8;ihdr[5]=c.h>>16;ihdr[4]=c.h>>24;ihdr[8]=8;ihdr[9]=2;chunk(o,"IHDR",ihdr);chunk(o,"IDAT",z);chunk(o,"IEND",{}); }

std::string escaparXml(const std::string& texto) {
    std::string resultado;
    for (const char caractere : texto) {
        switch (caractere) {
            case '&': resultado += "&amp;"; break;
            case '<': resultado += "&lt;"; break;
            case '>': resultado += "&gt;"; break;
            case '"': resultado += "&quot;"; break;
            default: resultado += caractere; break;
        }
    }
    return resultado;
}

void escreverNoSvg(std::ostream& saida, const Node& no) {
    constexpr int espacamentoHorizontal = 240;
    constexpr int espacamentoVertical = 90;
    constexpr int larguraNo = 220;
    constexpr int alturaNo = 38;
    const int x = no.x * espacamentoHorizontal + espacamentoHorizontal / 2;
    const int y = no.y * espacamentoVertical + 35;

    for (const Node& filho : no.children) {
        const int xFilho = filho.x * espacamentoHorizontal + espacamentoHorizontal / 2;
        const int yFilho = filho.y * espacamentoVertical + 35;
        saida << "<line stroke=\"#607080\" stroke-width=\"1.5\" x1=\"" << x << "\" y1=\"" << y + alturaNo / 2
              << "\" x2=\"" << xFilho << "\" y2=\"" << yFilho - alturaNo / 2
              << "\" />\n";
        escreverNoSvg(saida, filho);
    }

    saida << "<rect fill=\"#f7fbff\" stroke=\"#23638c\" x=\"" << x - larguraNo / 2 << "\" y=\"" << y - alturaNo / 2
          << "\" width=\"" << larguraNo << "\" height=\"" << alturaNo << "\" />\n"
          << "<text font-family=\"DejaVu Sans Mono\" font-size=\"14\" fill=\"#192a3a\" text-anchor=\"middle\" x=\"" << x << "\" y=\"" << y + 5 << "\">"
          << escaparXml(no.label) << "</text>\n";
}

void gerarPngComFonteDoSistema(const std::filesystem::path& caminhoPng, Node raiz) {
    int folhas = 0;
    int profundidade = 0;
    layout(raiz, 0, folhas, profundidade);

    const std::filesystem::path caminhoSvg = caminhoPng.parent_path() /
                                             (caminhoPng.stem().string() + ".tmp.svg");
    std::ofstream svg(caminhoSvg);
    if (!svg) throw std::runtime_error("Nao foi possivel criar a arvore temporaria.");

    svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << std::max(900, folhas * 240)
        << "\" height=\"" << std::max(160, (profundidade + 1) * 90 + 50) << "\">\n";
    escreverNoSvg(svg, raiz);
    svg << "</svg>\n";
    svg.close();

    const pid_t processo = fork();
    if (processo == 0) {
        execlp("convert", "convert", "-background", "white", "-density", "144",
               caminhoSvg.c_str(), caminhoPng.c_str(), static_cast<char*>(nullptr));
        _exit(127);
    }
    if (processo < 0) throw std::runtime_error("Nao foi possivel iniciar o conversor de imagens.");

    int status = 0;
    waitpid(processo, &status, 0);
    std::filesystem::remove(caminhoSvg);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        throw std::runtime_error("Nao foi possivel gerar o PNG da arvore. O ImageMagick e necessario.");
    }
}
}

std::filesystem::path GeradorArtefatos::gerar(const std::string& caminhoFonte,
                                               const std::string& nomePrograma,
                                               const No& arvoreSintatica,
                                               const Programa& ast) const {
    const std::filesystem::path dir = std::filesystem::path("artefatos") /
                                      (std::filesystem::path(nomePrograma).stem().string() + "-artefatos");
    std::filesystem::create_directories(dir);

    std::ofstream out(dir / "tokens.txt");
    if (!out) {
        throw std::runtime_error("Nao foi possivel criar a lista de tokens.");
    }
    Lexico lexico(caminhoFonte);
    while (true) {
        const Token token = lexico.proximoToken();
        if (token.tipo == TipoToken::FIMARQUIVO) break;
        out << tipoToken(token.tipo) << " | " << token.lexema
            << " | linha " << token.linha << ", coluna " << token.coluna << '\n';
    }
    gerarPngComFonteDoSistema(dir / "arvore-sintatica.png", concreto(arvoreSintatica));
    gerarPngComFonteDoSistema(dir / "ast.png", abstrata(ast));
    return dir;
}
