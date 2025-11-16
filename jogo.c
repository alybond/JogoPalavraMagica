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
    printf("=============================================\n");
    printf("           A D I V I N H A P A L A V R A     \n");
    printf("=============================================\n");
    printf("   Descubra a palavra a partir da dica!      \n");
    printf("=============================================\n\n");
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
    printf("COMO JOGAR\n\n");
    printf("- Escolha um tema (Princesas, Aventura ou Animais).\n");
    printf("- O jogo sorteia uma palavra secreta desse tema.\n");
    printf("- Voce ve uma dica e o numero de letras.\n");
    printf("- Digite uma letra por vez para tentar adivinhar.\n");
    printf("- Cada letra correta vale pontos, cada erro custa tentativas.\n");
    printf("- Ha um limite de tempo e de tentativas.\n");
    printf("- Se voce descobrir a palavra antes do tempo acabar, vence a partida.\n");
    printf("\nDivirta-se e exercite o raciocinio e o vocabulario!\n\n");
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

    limparTela();
    exibirTitulo();
    printf("RANKING\n\n");

    if (!f) {
        printf("Nenhum registro de ranking encontrado ainda.\n\n");
        pausar();
        return;
    }

    printf("Nome;Tema;Palavra;Pontos;Tempo(s)\n");
    printf("--------------------------------------------\n");

    while (fgets(linha, sizeof(linha), f) != NULL) {
        printf("%s", linha);
    }

    fclose(f);
    printf("\n");
    pausar();
}

/* =================== LÓGICA DO JOGO =================== */

void jogarPartida(const Palavra *p) {
    int i;
    int tamanho = (int)strlen(p->palavra);
    char exibicao[MAX_PALAVRA];
    int tentativasRestantes = p->tentativasMax;
    int pontuacao = 0;
    int letrasRestantes = 0;
    int letrasJaVisiveis = p->letrasVisiveis;
    int usadas[26] = {0};
    time_t inicio, agora;
    int tempoGasto;

    // preparar exibicao
    for (i = 0; i < tamanho; i++) {
        if (p->palavra[i] == ' ') {
            exibicao[i] = ' ';
        } else {
            exibicao[i] = '_';
        }
    }
    exibicao[tamanho] = '\0';

    // revelar letras visiveis
    if (letrasJaVisiveis > 0 && letrasJaVisiveis < tamanho) {
        int cont = 0;
        while (cont < letrasJaVisiveis) {
            int idx = rand() % tamanho;
            if (exibicao[idx] == '_') {
                exibicao[idx] = p->palavra[idx];
                cont++;
            }
        }
    }

    // contar letras restantes
    letrasRestantes = 0;
    for (i = 0; i < tamanho; i++) {
        if (exibicao[i] == '_')
            letrasRestantes++;
    }

    limparTela();
    exibirTitulo();
    printf("TEMA: %s\n", p->tema);
    printf("Nivel: %s | Tempo maximo: %d s | Tentativas: %d\n\n",
           p->nivel, p->tempoMax, p->tentativasMax);
    printf("DICA: %s\n\n", p->dica);

    time(&inicio);

    while (tentativasRestantes > 0 && letrasRestantes > 0) {
        char letra;
        int acerto = 0;

        time(&agora);
        tempoGasto = (int)difftime(agora, inicio);

        if (tempoGasto >= p->tempoMax) {
            printf("\nO tempo acabou!\n");
            break;
        }

        printf("\nPalavra: ");
        for (i = 0; i < tamanho; i++) {
            printf("%c ", exibicao[i]);
        }
        printf("\n");

        printf("Tentativas restantes: %d | Pontuacao: %d | Tempo: %ds\n",
               tentativasRestantes, pontuacao, tempoGasto);

        printf("Digite uma letra: ");
        letra = (char)getchar();
        while (getchar() != '\n'); // limpar buffer

        letra = (char)toupper((unsigned char)letra);

        if (letra < 'A' || letra > 'Z') {
            printf("Digite apenas letras de A a Z.\n");
            continue;
        }

        int indiceLetra = letra - 'A';
        if (usadas[indiceLetra]) {
            printf("Voce ja tentou essa letra. Escolha outra.\n");
            continue;
        }
        usadas[indiceLetra] = 1;

        // verifica se existe na palavra
        for (i = 0; i < tamanho; i++) {
            if (p->palavra[i] == letra && exibicao[i] == '_') {
                exibicao[i] = letra;
                acerto = 1;
                pontuacao += 10;
                letrasRestantes++;
                letrasRestantes--; // (mantem logica clara, mas poderiamos so decrementar)
                letrasRestantes--;
            }
        }

        // correção: vamos recalcular letrasRestantes direito
        letrasRestantes = 0;
        for (i = 0; i < tamanho; i++) {
            if (exibicao[i] == '_')
                letrasRestantes++;
        }

        if (acerto) {
            printf("Muito bem! Voce acertou uma letra!\n");
        } else {
            printf("Essa letra nao existe na palavra.\n");
            pontuacao -= 5;
            if (pontuacao < 0) pontuacao = 0;
            tentativasRestantes--;
        }
    }

    time(&agora);
    tempoGasto = (int)difftime(agora, inicio);

    if (letrasRestantes == 0) {
        printf("\nPARABENS! Voce descobriu a palavra: %s\n", p->palavra);
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
        printf("\nSuas tentativas acabaram.\n");
        printf("A palavra era: %s\n", p->palavra);
    } else {
        printf("\nO tempo acabou.\n");
        printf("A palavra era: %s\n", p->palavra);
    }

    pausar();
}
