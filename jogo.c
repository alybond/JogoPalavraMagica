#include "jogo.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif

// -------- utilidades internas --------
static void limpar_newline(char *s) {
    size_t n = strlen(s);
    while (n && (s[n-1] == '\n' || s[n-1] == '\r')) {
        s[--n] = '\0';
    }
}

void normalizar_maiusculas(char *s) {
    for (size_t i = 0; s[i]; ++i) {
        unsigned char c = (unsigned char)s[i];
        if (c >= 'a' && c <= 'z') s[i] = (char)toupper(c);
    }
}

// -------- arquivo de palavras --------
int carregar_palavras(const char *caminho, Entrada lista[], int max) {
    FILE *f = fopen(caminho, "r");
    if (!f) {
        perror("Erro ao abrir arquivo de palavras");
        return 0;
    }
    int qtd = 0;
    char linha[MAX_LINHA];
    while (qtd < max && fgets(linha, sizeof(linha), f)) {
        limpar_newline(linha);
        if (linha[0] == '\0' || linha[0] == '#') continue;

        // Formatos:
        // PALAVRA;DICA
        // PALAVRA;DICA;VISIVEIS
        char *p1 = strchr(linha, ';');
        if (!p1) continue;
        *p1 = '\0';
        char *p2 = strchr(p1 + 1, ';');

        strncpy(lista[qtd].palavra, linha, MAX_PALAVRA - 1);
        lista[qtd].palavra[MAX_PALAVRA - 1] = '\0';
        normalizar_maiusculas(lista[qtd].palavra);

        if (p2) { // com terceira coluna
            *p2 = '\0';
            strncpy(lista[qtd].dica, p1 + 1, MAX_DICA - 1);
            lista[qtd].dica[MAX_DICA - 1] = '\0';
            lista[qtd].visiveis = atoi(p2 + 1);
            if (lista[qtd].visiveis < 0) lista[qtd].visiveis = 0;
        } else {
            strncpy(lista[qtd].dica, p1 + 1, MAX_DICA - 1);
            lista[qtd].dica[MAX_DICA - 1] = '\0';
            lista[qtd].visiveis = 0; // padrão
        }
        qtd++;
    }
    fclose(f);
    return qtd;
}

void escolher_aleatoria(Entrada lista[], int qtd, Entrada *saida) {
    srand((unsigned)time(NULL));
    if (qtd <= 0) {
        saida->palavra[0] = '\0';
        saida->dica[0] = '\0';
        saida->visiveis = 0;
        return;
    }
    int idx = rand() % qtd;
    *saida = lista[idx];
}

// -------- jogo --------
void iniciar_jogo(Jogo *j, const Entrada *e, int tentativas) {
    memset(j, 0, sizeof(*j));
    j->atual = *e;
    j->tentativas_restantes = tentativas;
    j->pontuacao = 0;
    j->venceu = false;
    memset(j->usadas, 0, sizeof(j->usadas));
    j->acertos_total = 0;
    j->erros_total = 0;
    j->nivel = NIVEL_FACIL;
    j->tempo_limite_seg = 0;
    j->inicio = 0;
    j->tema = TEMA_PRINCESAS; // default

    size_t n = strlen(e->palavra);
    for (size_t i = 0; i < n; ++i) {
        if (e->palavra[i] == ' ' || e->palavra[i] == '-') {
            j->exibida[i] = e->palavra[i];
        } else {
            j->exibida[i] = '_';
        }
    }
    j->exibida[n] = '\0';

    // Revela até 'visiveis' letras em posições aleatórias
    if (e->visiveis > 0) {
        int alvo = e->visiveis;
        int guard = 1000; // evita loop infinito
        while (alvo > 0 && guard-- > 0) {
            size_t i = (size_t)(rand() % n);
            if (j->exibida[i] == '_' && isalpha((unsigned char)e->palavra[i])) {
                j->exibida[i] = e->palavra[i];
                j->letras_reveladas++;
                alvo--;
            }
        }
    }

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

void configurar_dificuldade(Jogo *j, Dificuldade nivel) {
    j->nivel = nivel;
    switch (nivel) {
        case NIVEL_FACIL:
            j->tentativas_restantes = 8;
            j->tempo_limite_seg = 0;  // sem tempo
            break;
        case NIVEL_MEDIO:
            j->tentativas_restantes = 7;
            j->tempo_limite_seg = 90; // 1m30s
            break;
        case NIVEL_DIFICIL:
            j->tentativas_restantes = 6;
            j->tempo_limite_seg = 60; // 1m
            break;
        default:
            j->tentativas_restantes = 8;
            j->tempo_limite_seg = 0;
    }
}

void configurar_tema(Jogo *j, Tema tema) { j->tema = tema; }

void iniciar_cronometro(Jogo *j) { j->inicio = time(NULL); }

int tempo_restante(const Jogo *j) {
    if (j->tempo_limite_seg <= 0 || j->inicio == 0) return 9999; // “sem tempo”
    time_t agora = time(NULL);
    int decorrido = (int)difftime(agora, j->inicio);
    int restante = j->tempo_limite_seg - decorrido;
    return restante > 0 ? restante : 0;
}

// retorna 1 se a entrada é letra A..Z (válida para o jogo)
int tentar_letra(Jogo *j, char letra, int *ocorrencias, int *repetida) {
    *ocorrencias = 0;
    *repetida = 0;

    letra = (char)toupper((unsigned char)letra);
    if (letra < 'A' || letra > 'Z') return 0;

    int idx = letra - 'A';
    if (j->usadas[idx]) {
        *repetida = 1;
        return 1; // válida, mas já usada
    }
    j->usadas[idx] = 1;

    for (size_t i = 0; j->atual.palavra[i]; ++i) {
        if (j->atual.palavra[i] == letra && j->exibida[i] == '_') {
            j->exibida[i] = letra;
            (*ocorrencias)++;
        }
    }

    if (*ocorrencias > 0) {
        j->pontuacao += 10 * (*ocorrencias);
        j->letras_reveladas += *ocorrencias;
        j->acertos_total += *ocorrencias;   // registra acertos
    } else {
        j->pontuacao -= 5;
        j->tentativas_restantes--;
        j->erros_total += 1;                // registra um erro
    }

    if (strcmp(j->exibida, j->atual.palavra) == 0) {
        j->venceu = true;
    }
    return 1;
}

bool terminou(const Jogo *j) {
    if (j->venceu) return true;
    if (j->tentativas_restantes <= 0) return true;
    if (j->tempo_limite_seg > 0 && j->inicio > 0 && tempo_restante(j) <= 0) return true;
    return false;
}

static void listar_usadas(const Jogo *j, char *buf, size_t buflen) {
    size_t pos = 0;
    for (int i = 0; i < 26 && pos + 3 < buflen; ++i) {
        if (j->usadas[i]) {
            buf[pos++] = (char)('A' + i);
            buf[pos++] = ' ';
        }
    }
    buf[pos] = '\0';
}

void desenhar(const Jogo *j) {
    int t = tempo_restante(j);
    char usadas[128]; usadas[0] = '\0';
    listar_usadas(j, usadas, sizeof(usadas));

    printf("\n=== PALAVRA MÁGICA – O JOGO DAS CRIANCAS ===\n");
    printf("Dica: %s\n", j->atual.dica);
    printf("Palavra: %s\n", j->exibida);

    printf("Tentativas: %d  | Pontos: %d", j->tentativas_restantes, j->pontuacao);
    if (j->tempo_limite_seg > 0) {
        printf("  | Tempo: %ds", t == 9999 ? j->tempo_limite_seg : t);
    }
    printf("  | Acertos: %d  | Erros: %d\n", j->acertos_total, j->erros_total);

    if (usadas[0]) printf("Letras usadas: %s\n", usadas);

    if (j->venceu) {
        if (j->tema == TEMA_AVENTURAS) {
            printf("\n🏆 Parabens, heroi! Voce conseguiu!\n");
        } else {
            printf("\n👑 Parabens, princesa! Voce conseguiu!\n");
        }
    } else if (j->tentativas_restantes <= 0) {
        printf("\nAcabaram as tentativas. A palavra era: %s\n", j->atual.palavra);
    } else if (j->tempo_limite_seg > 0 && t <= 0) {
        printf("\nO tempo se esgotou! A palavra era: %s\n", j->atual.palavra);
    }
}

void mostrar_regras(void) {
    printf("\n===== COMO JOGAR =====\n");
    printf("- Voce vera uma dica e tracos representando as letras.\n");
    printf("- Digite UMA letra por vez (A-Z).\n");
    printf("- Acerto: +10 pontos por ocorrencia da letra.\n");
    printf("- Erro: -5 pontos e voce perde 1 tentativa.\n");
    printf("- Vence ao descobrir todas as letras.\n");
    printf("- Se acabarem as tentativas (ou o tempo do nivel), voce perde.\n");
    printf("======================\n\n");
}
