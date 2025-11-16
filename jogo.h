#ifndef JOGO_H
#define JOGO_H

// arquivos usados pelo jogo
#define ARQUIVO_PALAVRAS "palavras.txt"
#define ARQUIVO_RANKING  "ranking.txt"

// limites de tamanho e quantidade
#define MAX_PALAVRAS   200   // quantidade maxima de palavras no jogo
#define MAX_TEMA       20    // tamanho maximo do texto do tema
#define MAX_PALAVRA    32    // tamanho maximo da palavra secreta
#define MAX_DICA       200   // tamanho maximo da dica
#define MAX_NIVEL      10    // tamanho maximo do nivel

// estrutura que representa uma palavra do jogo
typedef struct {
    char tema[MAX_TEMA];       // texto do tema (ex: PRINCESA)
    char palavra[MAX_PALAVRA]; // palavra secreta (ex: CINDERELA)
    char dica[MAX_DICA];       // dica da palavra
    char nivel[MAX_NIVEL];     // FACIL / MEDIO / DIFICIL
    int letrasVisiveis;        // letras mostradas no inicio
    int tempoMax;              // tempo maximo em segundos
    int tentativasMax;         // numero maximo de tentativas
} Palavra;

// funcoes simples de interface com o usuario
void limparTela();
void pausar();

// funcoes para carregar e filtrar palavras do arquivo
int carregarPalavras(const char *nomeArquivo, Palavra vetor[], int *qtdTotal);
int filtrarPorTema(const Palavra origem[], int qtdOrigem, const char *tema,
                   Palavra destino[], int *qtdDestino);
int sortearIndice(int limite);

// interface de texto
void exibirTitulo();
void exibirMenuPrincipal();
void exibirComoJogar();
void exibirRanking();

// interface grafica (Windows Forms)
void mostrarInterfaceTema(const Palavra *p);
void mostrarInterfaceAcerto();
void mostrarInterfaceErro();
void mostrarInterfaceParabens();
void mostrarInterfaceGameOver();

// ranking
void registrarRanking(const char *nomeJogador, const Palavra *p, int pontuacao, int tempoGasto);

// logica do jogo
void jogarPartida(const Palavra *p);

#endif // JOGO_H
