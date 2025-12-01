#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "jogo.h"

/* =================== funcoes de utilidade =================== */

// funcao para limpar a tela
void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// chama o executavel da interface C# passando um comando
static void abrirInterface(const char *comando) {
#ifdef _WIN32
    char cmd[512];
    // ajuste o caminho se o seu FormInterface gerar em outra pasta
    snprintf(
        cmd,
        sizeof(cmd),
        "start \"\" \"FormInterface\\bin\\Debug\\net8.0-windows\\FormInterface.exe\" %s",
        comando
    );
    system(cmd);
#else
    (void)comando;
#endif
}

// mostra a imagem do tema via FormInterface
void mostrarInterfaceTema(const Palavra *p) {
    abrirInterface(p->tema);  // PRINCESA / ANIMAL / AVENTURA
}

void mostrarInterfaceAcerto() {
    abrirInterface("acerto");
}

void mostrarInterfaceErro() {
    abrirInterface("erro");
}

void mostrarInterfaceParabens() {
    abrirInterface("parabens");
}

void mostrarInterfaceGameOver() {
    abrirInterface("gameover");
}

// pausa ate ENTER
void pausar() {
    printf("\nPressione ENTER para continuar...");
    getchar();
}

// converte string para maiusculas
static void strToUpper(char *s) {
    while (*s) {
        *s = (char)toupper((unsigned char)*s);
        s++;
    }
}

// remove \n do final de string
static void removeQuebraLinha(char *s) {
    size_t len = strlen(s);
    if (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
    }
}

/* =================== carregamento de dados =================== */

int carregarPalavras(const char *nomeArquivo, Palavra vetor[], int *qtdTotal) {
    FILE *f;
    char linha[512];
    int contador;

    f = fopen(nomeArquivo, "r");
    contador = 0;

    if (!f) {
        printf("Erro ao abrir o arquivo de palavras (%s).\n", nomeArquivo);
        return 0;
    }

    while (fgets(linha, sizeof(linha), f) != NULL && contador < MAX_PALAVRAS) {
        if (linha[0] == '\n' || linha[0] == '\0') {
            continue;
        }

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

        strToUpper(p.tema);
        strToUpper(p.palavra);
        strToUpper(p.nivel);

        vetor[contador] = p;
        contador++;
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
    int i;
    int j = 0;

    for (i = 0; i < qtdOrigem; i++) {
        if (strcmp(origem[i].tema, tema) == 0) {
            destino[j] = origem[i];
            j++;
        }
    }

    *qtdDestino = j;
    return (j > 0);
}

int sortearIndice(int limite) {
    if (limite <= 0) return -1;
    return rand() % limite;
}

/* =================== interface de texto =================== */

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

/* =================== ranking =================== */

void registrarRanking(const char *nomeJogador, const Palavra *p, int pontuacao, int tempoGasto) {
    FILE *f = fopen(ARQUIVO_RANKING, "a");
    if (!f) {
        printf("Nao foi possivel registrar ranking.\n");
        return;
    }

    fprintf(f, "%s;%s;%s;%d;%d\n",
            nomeJogador, p->tema, p->palavra, pontuacao, tempoGasto);

    fclose(f);
}

void exibirRanking() {
    FILE *f;
    char linha[256];
    int pos;

    limparTela();
    exibirTitulo();
    printf("RANKING\n\n");

    f = fopen(ARQUIVO_RANKING, "r");
    if (!f) {
        printf("Nenhum registro de ranking encontrado ainda.\n\n");
        pausar();
        return;
    }

    printf(" #  %-15s %-10s %-12s %-8s %-8s\n",
           "Nome", "Tema", "Palavra", "Pontos", "Tempo");
    printf("---------------------------------------------------------------\n");

    pos = 1;

    while (fgets(linha, sizeof(linha), f) != NULL) {
        char *token;
        char nome[50];
        char tema[20];
        char palavra[32];
        int pontos;
        int tempo;

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

/* =================== logica do jogo =================== */

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
    int tamanho;
    char exibicao[MAX_PALAVRA];
    int tentativasRestantes;
    int pontuacao;
    int letrasRestantes;
    int letrasJaVisiveis;
    int usadas[26];
    int dicas[26];
    time_t inicio;
    time_t agora;
    int tempoGasto;

    tamanho = (int)strlen(p->palavra);

    // mostra imagem do tema no inicio
    mostrarInterfaceTema(p);

    tentativasRestantes = p->tentativasMax;
    pontuacao = 0;
    letrasRestantes = 0;
    letrasJaVisiveis = p->letrasVisiveis;

    for (i = 0; i < 26; i++) {
        usadas[i] = 0;
        dicas[i] = 0;
    }

    for (i = 0; i < tamanho; i++) {
        if (p->palavra[i] == ' ') {
            exibicao[i] = ' ';
        } else {
            exibicao[i] = '_';
        }
    }
    exibicao[tamanho] = '\0';

    if (letrasJaVisiveis > 0 && letrasJaVisiveis < tamanho) {
        int cont = 0;
        while (cont < letrasJaVisiveis) {
            int idx = rand() % tamanho;

            if (exibicao[idx] == '_' && p->palavra[idx] != ' ') {
                exibicao[idx] = p->palavra[idx];
                cont++;

                if (p->palavra[idx] >= 'A' && p->palavra[idx] <= 'Z') {
                    int li = p->palavra[idx] - 'A';
                    dicas[li] = 1;
                }
            }
        }
    }

    letrasRestantes = 0;
    for (i = 0; i < tamanho; i++) {
        if (exibicao[i] == '_') {
            letrasRestantes++;
        }
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
            mostrarInterfaceGameOver();
            break;
        }

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
            ch = getchar();
            while (ch != '\n' && ch != EOF) {
                ch = getchar();
            }
            printf("Entrada invalida.\n");
            pausar();
            continue;
        }

        {
            int ch;
            ch = getchar();
            while (ch != '\n' && ch != EOF) {
                ch = getchar();
            }
        }

        letra = (char)toupper((unsigned char)letra);

        if (letra < 'A' || letra > 'Z') {
            printf("Digite apenas letras de A a Z.\n");
            pausar();
            continue;
        }

        {
            int indiceLetra = letra - 'A';
            if (usadas[indiceLetra]) {
                printf("Voce ja tentou a letra '%c'.\n", letra);
                exibirLetrasUsadas(usadas, dicas);
                pausar();
                continue;
            }
            usadas[indiceLetra] = 1;
        }

        for (i = 0; i < tamanho; i++) {
            if (p->palavra[i] == letra && exibicao[i] == '_') {
                exibicao[i] = letra;
                acerto = 1;
                pontuacao += 10;
                letrasRestantes--;
            }
        }

        if (!acerto) {
            printf("\nEssa letra nao existe na palavra.\n");
            mostrarInterfaceErro();
            pontuacao -= 5;
            if (pontuacao < 0) pontuacao = 0;
            tentativasRestantes--;
            pausar();
        } else {
            printf("\nBoa! Voce acertou uma letra.\n");
            mostrarInterfaceAcerto();
            pausar();
        }
    }

    time(&agora);
    tempoGasto = (int)difftime(agora, inicio);

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

        mostrarInterfaceParabens();

        {
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
        }
    } else if (tentativasRestantes == 0) {
        printf("Suas tentativas acabaram.\n");
        printf("A palavra era: %s\n", p->palavra);
        mostrarInterfaceGameOver();
    } else {
        printf("O tempo acabou.\n");
        printf("A palavra era: %s\n", p->palavra);
        mostrarInterfaceGameOver();
    }

    pausar();
}
