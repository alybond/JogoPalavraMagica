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
    char tema[MAX_TEMA];       // texto do tema )
    char palavra[MAX_PALAVRA]; // texto da palavra secreta (ex: cinderela)
    char dica[MAX_DICA];       // texto da dica da palavra
    char nivel[MAX_NIVEL];     // nivel de dificuldade (ex: facil / medio / dificil)
    int letrasVisiveis;        // quantidade de letras mostradas no inicio como dica
    int tempoMax;              // tempo maximo para descobrir a palavra em segundos
    int tentativasMax;         // numero maximo de tentativas de letra
} Palavra;

// funcoes simples de interface com o usuario
void limparTela();  // limpa o texto da tela
void pausar();      // espera o usuario apertar enter

// funcoes para carregar e filtrar palavras do arquivo
int carregarPalavras(const char *nomeArquivo, Palavra vetor[], int *qtdTotal);
// filtra palavras por tema e salva em outro vetor
int filtrarPorTema(const Palavra origem[], int qtdOrigem, const char *tema,
                   Palavra destino[], int *qtdDestino);
// sorteia um indice aleatorio de 0 ate limite - 1
int sortearIndice(int limite);

// funcoes de interface de menu e explicacoes
void exibirTitulo();         // mostra o titulo do jogo
void exibirMenuPrincipal();  // mostra o menu principal com os temas
void exibirComoJogar();      // mostra instrucoes de como jogar
void exibirRanking();        // mostra o ranking salvo no arquivo

// funcoes para gravar e controlar o ranking
void registrarRanking(const char *nomeJogador, const Palavra *p, int pontuacao, int tempoGasto);

// logica principal de uma partida do jogo com uma palavra
void jogarPartida(const Palavra *p);

#endif // JOGO_H
