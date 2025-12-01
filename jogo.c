#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "jogo.h"

/* =================== FUNÇÕES DE UTILIDADE =================== */

void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausar() {
    printf("\nPressione ENTER para continuar...");
    getchar();
}

// transforma string em maiúsculas
static void strToUpper(char *s) {
    while (*s) {
        *s = (char)toupper((unsigned char)*s);
        s++;
    }
}

// remove \n do final de string lida com fgets
static void removeQuebraLinha(char *s) {
    size_t len = strlen(s);
    if (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
    }
}

/* =================== CARREGAMENTO DE DADOS =================== */

int carregarPalavras(const char *nomeArquivo, Palavra vetor[], int *qtdTotal) {
    FILE *f = fopen(nomeArquivo, "r");
    char linha[512];
    int contador = 0;

    if (!f) {
        printf("Erro ao abrir o arquivo de palavras (%s).\n", nomeArquivo);
        return 0;
    }

    while (fgets(linha, sizeof(linha), f) != NULL && contador < MAX_PALAVRAS) {
        if (linha[0] == '\n' || linha[0] == '\0')
            continue; // linha vazia

        Palavra p;
        char *token;

        token = strtok(linha, ";\n\r");
        if (!token) continue;
        strncpy(p.tema, token, MAX_TEMA);
        p.tema[MAX_TEMA - 1] = '\0';

        token = strtok(NULL, ";\n\r");
        if (!token) continue;
        strncpy(p.palavra, token, MAX_PALAVRA);
        p.palavra[MAX_PALAVRA - 1] = '\0';

        token = strtok(NULL, ";\n\r");
        if (!token) continue;
        strncpy(p.dica, token, MAX_DICA);
        p.dica[MAX_DICA - 1] = '\0';

        token = strtok(NULL, ";\n\r");
        if (!token) continue;
        strncpy(p.nivel, token, MAX_NIVEL);
        p.nivel[MAX_NIVEL - 1] = '\0';

        token = strtok(NULL, ";\n\r");
        if (!token) continue;
        p.letrasVisiveis = atoi(token);

        token = strtok(NULL, ";\n\r");
        if (!token) continue;
        p.tempoMax = atoi(token);

        token = strtok(NULL, ";\n\r");
        if (!token) continue;
        p.tentativasMax = atoi(token);

        // normalizar em maiúsculas
        strToUpper(p.tema);
        strToUpper(p.palavra);
        strToUpper(p.nivel);

        vetor[contador++] = p;
    }

    fclose(f);
    *qtdTotal = contador;

    if (contador == 0) {
        printf("Nenhuma palavra foi carregada do arquivo.\n");
        return 0;
    }

    return 1;
}

int filtrarPorTema(const Palavra origem[], int qtdOrigem, const char *tema,
                   Palavra destino[], int *qtdDestino) {
    int i, j = 0;

    for (i = 0; i < qtdOrigem; i++) {
        if (strcmp(origem[i].tema, tema) == 0) {
            destino[j++] = origem[i];
        }
    }

    *qtdDestino = j;
    return (j > 0);
}

int sortearIndice(int limite) {
    if (limite <= 0) return -1;
    return rand() % limite;
}

/* =================== INTERFACE (TEXTO) =================== */

void exibirTitulo() {
    printf("\n");
    printf("*           *       .     *       .      *  \n");
    printf("      *         ADIVINHAPALAVRA          . \n");
    printf("         .                .       *           *  \n");
    printf("   .          O jogo das palavras      *\n");
    printf("           *       .     *       .        \n");
    printf("    *       .          .                 .       *  \n");


 
 
    printf("\n");
}

void exibirMenuPrincipal() {
    exibirTitulo();
    printf("Escolha um tema:\n\n");
    printf("  1) Princesas\n");
    printf("  2) Aventura\n");
    printf("  3) Animais\n");
    printf("  4) Como jogar\n");
    printf("  5) Ver ranking\n");
    printf("  0) Sair\n\n");
    printf("Opcao: ");
}

void exibirComoJogar() {
    limparTela();
    exibirTitulo();
    printf("COMO JOGAR: \n");
    printf("- Escolha um tema (Princesas, Aventura ou Animais).\n");
    printf("- O jogo sorteia uma palavra secreta desse tema.\n");
    printf("- Voce ve uma dica e o numero de letras.\n");
    printf("- Digite uma letra por vez para tentar adivinhar.\n");
    printf("- Cada letra correta vale pontos, cada erro custa tentativas.\n");
    printf("- Ha um limite de tempo e de tentativas.\n");
    printf("- Se voce descobrir a palavra antes do tempo acabar, vence a partida.\n");
    pausar();
}

/* =================== RANKING =================== */

void registrarRanking(const char *nomeJogador, const Palavra *p, int pontuacao, int tempoGasto) {
    FILE *f = fopen(ARQUIVO_RANKING, "a");

    if (!f) {
        printf("Nao foi possivel registrar ranking.\n");
        return;
    }

    fprintf(f, "%s;%s;%s;%d;%d\n",
            nomeJogador,
            p->tema,
            p->palavra,
            pontuacao,
            tempoGasto);

    fclose(f);
}

void exibirRanking() {
    FILE *f = fopen(ARQUIVO_RANKING, "r");
    char linha[256];
    int pos = 1;

    limparTela();
    exibirTitulo();
    printf("RANKING\n\n");

    if (!f) {
        printf("Nenhum registro de ranking encontrado ainda.\n\n");
        pausar();
        return;
    }

    printf(" #  %-15s %-10s %-12s %-8s %-8s\n",
           "Nome", "Tema", "Palavra", "Pontos", "Tempo");
    printf("---------------------------------------------------------------\n");

    while (fgets(linha, sizeof(linha), f) != NULL) {
        char *token;
        char nome[50], tema[20], palavra[32];
        int pontos, tempo;

        token = strtok(linha, ";\n\r");
        if (!token) continue;
        strncpy(nome, token, sizeof(nome));
        nome[sizeof(nome) - 1] = '\0';

        token = strtok(NULL, ";\n\r");
        if (!token) continue;
        strncpy(tema, token, sizeof(tema));
        tema[sizeof(tema) - 1] = '\0';

        token = strtok(NULL, ";\n\r");
        if (!token) continue;
        strncpy(palavra, token, sizeof(palavra));
        palavra[sizeof(palavra) - 1] = '\0';

        token = strtok(NULL, ";\n\r");
        if (!token) continue;
        pontos = atoi(token);

        token = strtok(NULL, ";\n\r");
        if (!token) continue;
        tempo = atoi(token);

        printf("%2d  %-15s %-10s %-12s %-8d %-8d\n",
               pos, nome, tema, palavra, pontos, tempo);
        pos++;
    }

    fclose(f);
    printf("\n");
    pausar();
}

/* =================== LÓGICA DO JOGO =================== */

static void exibirLetrasUsadas(int usadas[26], int dicas[26]) {
    int i;
    int primeiro = 1;

    printf("Letras ja mostradas: ");
    for (i = 0; i < 26; i++) {
        if (usadas[i] || dicas[i]) {
            if (!primeiro) {
                printf(", ");
            }
            printf("%c", 'A' + i);
            primeiro = 0;
        }
    }
    if (primeiro) {
        printf("nenhuma ainda");
    }
    printf("\n");
}

void jogarPartida(const Palavra *p) {
    int i;
    int tamanho = (int)strlen(p->palavra);
    char exibicao[MAX_PALAVRA];
    int tentativasRestantes = p->tentativasMax;
    int pontuacao = 0;
    int letrasRestantes = 0;
    int letrasJaVisiveis = p->letrasVisiveis;
    int usadas[26] = {0};  // letras que o jogador tentou
    int dicas[26]  = {0};  // letras reveladas como dica
    time_t inicio, agora;
    int tempoGasto;

    // preparar exibicao: '_' para letras, ' ' para espaco
    for (i = 0; i < tamanho; i++) {
        if (p->palavra[i] == ' ') {
            exibicao[i] = ' ';
        } else {
            exibicao[i] = '_';
        }
    }
    exibicao[tamanho] = '\0';

    // revelar letras visiveis iniciais (dica)
    if (letrasJaVisiveis > 0 && letrasJaVisiveis < tamanho) {
        int cont = 0;
        while (cont < letrasJaVisiveis) {
            int idx = rand() % tamanho;
            if (exibicao[idx] == '_' && p->palavra[idx] != ' ') {
                exibicao[idx] = p->palavra[idx];
                cont++;

                // marca essa letra como "dica", NAO como usada
                if (p->palavra[idx] >= 'A' && p->palavra[idx] <= 'Z') {
                    int li = p->palavra[idx] - 'A';
                    dicas[li] = 1;
                }
            }
        }
    }

    // contar letras que ainda faltam
    letrasRestantes = 0;
    for (i = 0; i < tamanho; i++) {
        if (exibicao[i] == '_')
            letrasRestantes++;
    }

    time(&inicio);

    while (tentativasRestantes > 0 && letrasRestantes > 0) {
        char letra;
        int acerto = 0;

        time(&agora);
        tempoGasto = (int)difftime(agora, inicio);

        if (tempoGasto >= p->tempoMax) {
            limparTela();
            exibirTitulo();
            printf("TEMA: %s\n", p->tema);
            printf("Nivel: %s | Tempo maximo: %d s | Tentativas: %d\n\n",
                   p->nivel, p->tempoMax, p->tentativasMax);
            printf("DICA: %s\n\n", p->dica);
            printf("\nO tempo acabou!\n");
            break;
        }

        // limpa a tela a cada rodada e redesenha tudo
        limparTela();
        exibirTitulo();
        printf("TEMA: %s\n", p->tema);
        printf("Nivel: %s | Tempo maximo: %d s | Tentativas: %d\n\n",
               p->nivel, p->tempoMax, p->tentativasMax);
        printf("DICA: %s\n\n", p->dica);

        printf("Palavra: ");
        for (i = 0; i < tamanho; i++) {
            printf("%c ", exibicao[i]);
        }
        printf("\n");

        printf("Tentativas restantes: %d | Pontuacao: %d | Tempo: %ds\n",
               tentativasRestantes, pontuacao, tempoGasto);

        exibirLetrasUsadas(usadas, dicas);

        printf("Digite uma letra: ");
        if (scanf(" %c", &letra) != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            printf("Entrada invalida.\n");
            pausar();
            continue;
        }
        // limpa qualquer resto da linha
        {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }

        letra = (char)toupper((unsigned char)letra);

        if (letra < 'A' || letra > 'Z') {
            printf("Digite apenas letras de A a Z.\n");
            pausar();
            continue;
        }

        int indiceLetra = letra - 'A';

        // agora, SÓ bloqueia se a letra ja foi TENTADA (usadas),
        // letras apenas de dica (dicas[]) NAO bloqueiam
        if (usadas[indiceLetra]) {
            printf("Voce ja tentou a letra '%c'.\n", letra);
            exibirLetrasUsadas(usadas, dicas);
            pausar();
            continue;
        }

        // marca como tentativa
        usadas[indiceLetra] = 1;

        // verifica se a letra existe na palavra
        for (i = 0; i < tamanho; i++) {
            if (p->palavra[i] == letra && exibicao[i] == '_') {
                exibicao[i] = letra;
                acerto = 1;
                pontuacao += 10;
                letrasRestantes--;  // uma letra a menos para descobrir
            }
        }

        if (!acerto) {
            printf("\nEssa letra nao existe na palavra.\n");
            pontuacao -= 5;
            if (pontuacao < 0) pontuacao = 0;
            tentativasRestantes--;
            pausar();
        }
    }

    time(&agora);
    tempoGasto = (int)difftime(agora, inicio);

    // tela final limpa
    limparTela();
    exibirTitulo();
    printf("TEMA: %s\n", p->tema);
    printf("Nivel: %s | Tempo maximo: %d s | Tentativas: %d\n\n",
           p->nivel, p->tempoMax, p->tentativasMax);
    printf("DICA: %s\n\n", p->dica);

    if (letrasRestantes == 0) {
        int bonusTentativas = tentativasRestantes * 2;
        int bonusTempo = (p->tempoMax - tempoGasto) / 5;
        if (bonusTempo < 0) bonusTempo = 0;

        printf("PARABENS! Voce descobriu a palavra: %s\n\n", p->palavra);
        printf("Pontuacao base: %d\n", pontuacao);
        printf("Bonus por tentativas restantes: %d\n", bonusTentativas);
        printf("Bonus por tempo: %d\n", bonusTempo);

        pontuacao += bonusTentativas + bonusTempo;

        printf("Pontuacao final: %d | Tempo utilizado: %ds\n", pontuacao, tempoGasto);

        char nome[50];
        printf("\nDigite seu nome para registrar no ranking (ou deixe vazio para pular): ");
        if (fgets(nome, sizeof(nome), stdin) != NULL) {
            removeQuebraLinha(nome);
            if (strlen(nome) > 0) {
                registrarRanking(nome, p, pontuacao, tempoGasto);
                printf("Registro salvo!\n");
            } else {
                printf("Registro nao salvo.\n");
            }
        }
    } else if (tentativasRestantes == 0) {
        printf("Suas tentativas acabaram.\n");
        printf("A palavra era: %s\n", p->palavra);
    } else {
        printf("O tempo acabou.\n");
        printf("A palavra era: %s\n", p->palavra);
    }

    pausar();
}

