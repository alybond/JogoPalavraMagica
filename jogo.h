#ifndef JOGO_H
#define JOGO_H

// Arquivos usados pelo jogo
#define ARQUIVO_PALAVRAS "palavras.txt"
#define ARQUIVO_RANKING  "ranking.txt"

// Limites
#define MAX_PALAVRAS   200
#define MAX_TEMA       20
#define MAX_PALAVRA    32
#define MAX_DICA       200
#define MAX_NIVEL      10

// Estrutura que representa uma palavra do jogo
typedef struct {
    char tema[MAX_TEMA];       // PRINCESA / ANIMAL / AVENTURA
    char palavra[MAX_PALAVRA]; // ex: CINDERELA
    char dica[MAX_DICA];       // frase de dica
    char nivel[MAX_NIVEL];     // FACIL / MEDIO / DIFICIL
    int letrasVisiveis;        // quantas letras aparecem no inicio
    int tempoMax;              // tempo maximo (segundos)
    int tentativasMax;         // tentativas maximas
} Palavra;

// Funções utilitárias
void limparTela();
void pausar();

// Funções de carregamento e filtro
int carregarPalavras(const char *nomeArquivo, Palavra vetor[], int *qtdTotal);
int filtrarPorTema(const Palavra origem[], int qtdOrigem, const char *tema,
                   Palavra destino[], int *qtdDestino);
int sortearIndice(int limite);

// Funções de interface
void exibirTitulo();
void exibirMenuPrincipal();
void exibirComoJogar();
void exibirRanking();

// Funções de ranking
void registrarRanking(const char *nomeJogador, const Palavra *p, int pontuacao, int tempoGasto);

// Lógica principal de um jogo
void jogarPartida(const Palavra *p);

#endif // JOGO_H
