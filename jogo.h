#ifndef JOGO_H
#define JOGO_H

#include <stdbool.h>
#include <time.h>

#define MAX_PALAVRA 64
#define MAX_DICA    128
#define MAX_LINHA   256
#define MAX_LISTA   512

typedef enum {
    NIVEL_FACIL = 1,
    NIVEL_MEDIO = 2,
    NIVEL_DIFICIL = 3
} Dificuldade;

typedef enum {
    TEMA_PRINCESAS = 1,
    TEMA_AVENTURAS = 2
} Tema;

typedef struct {
    char palavra[MAX_PALAVRA]; // ex: "CINDERELA" (sem acento)
    char dica[MAX_DICA];       // ex: "Perdeu o sapato no baile"
    int  visiveis;             // quantas letras já começam reveladas
} Entrada;

typedef struct {
    Entrada atual;
    char exibida[MAX_PALAVRA];
    int tentativas_restantes;
    int pontuacao;
    int letras_reveladas;
    bool venceu;

    // controle
    int usadas[26];            // letras já tentadas (A..Z)
    Dificuldade nivel;         // fácil/médio/difícil
    int tempo_limite_seg;      // 0 = sem tempo
    time_t inicio;             // início da partida
    Tema tema;                 // princesas/aventuras

    // contadores exigidos
    int acertos_total;         // ocorrências reveladas por chutes
    int erros_total;           // chutes sem ocorrência
} Jogo;

// === API base ===
int  carregar_palavras(const char *caminho, Entrada lista[], int max);
void escolher_aleatoria(Entrada lista[], int qtd, Entrada *saida);
void iniciar_jogo(Jogo *j, const Entrada *e, int tentativas);
bool terminou(const Jogo *j);
void desenhar(const Jogo *j);
void normalizar_maiusculas(char *s);

// === novas APIs ===
void configurar_dificuldade(Jogo *j, Dificuldade nivel);
void configurar_tema(Jogo *j, Tema tema);
void iniciar_cronometro(Jogo *j);
int  tempo_restante(const Jogo *j); // em segundos (0 quando acabou)
int  tentar_letra(Jogo *j, char letra, int *ocorrencias, int *repetida); // 1 = letra válida
void mostrar_regras(void);

#endif
