#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "jogo.h"

// Palavras por letra para frases tipo "A de Amor", "M de Milho"
static const char* palavra_por_letra(char c) {
    switch (toupper((unsigned char)c)) {
        case 'A': return "Amor";
        case 'B': return "Bola";
        case 'C': return "Casa";
        case 'D': return "Dado";
        case 'E': return "Elefante";
        case 'F': return "Fada";
        case 'G': return "Gato";
        case 'H': return "Helicoptero";
        case 'I': return "Igreja";
        case 'J': return "Jacare";
        case 'K': return "Kiwi";
        case 'L': return "Lua";
        case 'M': return "Milho";
        case 'N': return "Ninho";
        case 'O': return "Ovo";
        case 'P': return "Pipa";
        case 'Q': return "Queijo";
        case 'R': return "Rato";
        case 'S': return "Sol";
        case 'T': return "Tigre";
        case 'U': return "Uva";
        case 'V': return "Vaca";
        case 'W': return "Waffle";
        case 'X': return "Xicara";
        case 'Y': return "Yakisoba";
        case 'Z': return "Zebra";
        default:  return "";
    }
}

static Tema ler_tema(char *arquivo_out, size_t tam) {
    printf("Escolha o tema do jogo:\n");
    printf("1) Princesas (👑)\n");
    printf("2) Aventuras (🏰)\n");
    printf("Opcao: ");
    int c = getchar();
    while (c == '\n' || c == '\r') c = getchar();
    int tema = c - '0';
    // flush resto da linha
    int d;
    while ((d = getchar()) != '\n' && d != EOF) { }

    if (tema == 2) {
        strncpy(arquivo_out, "palavras_aventura.txt", tam);
        arquivo_out[tam-1] = '\0';
        return TEMA_AVENTURAS;
    } else {
        strncpy(arquivo_out, "palavras_princesas.txt", tam);
        arquivo_out[tam-1] = '\0';
        return TEMA_PRINCESAS;
    }
}

static Dificuldade ler_nivel(void) {
    printf("\nEscolha o nivel:\n");
    printf("1) Facil   (8 tentativas, sem tempo)\n");
    printf("2) Medio   (7 tentativas, 90s)\n");
    printf("3) Dificil (6 tentativas, 60s)\n");
    printf("Opcao: ");
    int c = getchar();
    while (c == '\n' || c == '\r') c = getchar();
    int nivel = c - '0';
    // flush resto da linha
    int d;
    while ((d = getchar()) != '\n' && d != EOF) { }
    if (nivel < 1 || nivel > 3) nivel = 1;
    return (Dificuldade)nivel;
}

int main(void) {
    // tema + arquivo
    char arquivo[64];
    Tema tema = ler_tema(arquivo, sizeof(arquivo));

    // carregar lista
    Entrada lista[MAX_LISTA];
    int qtd = carregar_palavras(arquivo, lista, MAX_LISTA);
    if (qtd <= 0) {
        printf("Nao foi possivel carregar '%s'.\n", arquivo);
        return 1;
    }

    // escolher palavra
    Entrada e;
    escolher_aleatoria(lista, qtd, &e);
    if (e.palavra[0] == '\0') {
        printf("Lista vazia.\n");
        return 1;
    }

    // regras
    mostrar_regras();

    // nivel
    Jogo jogo;
    iniciar_jogo(&jogo, &e, 8); // default
    configurar_tema(&jogo, tema);
    Dificuldade nivel = ler_nivel();
    configurar_dificuldade(&jogo, nivel);
    if (jogo.tempo_limite_seg > 0) iniciar_cronometro(&jogo);

    // loop principal
    while (!terminou(&jogo)) {
        desenhar(&jogo);

        printf("Digite uma letra: ");
        int c = getchar();
        while (c == '\n' || c == '\r') c = getchar();
        char letra = (char)c;

        int ocorrencias = 0, repetida = 0;
        int valida = tentar_letra(&jogo, letra, &ocorrencias, &repetida);

        // flush da linha
        int d;
        while ((d = getchar()) != '\n' && d != EOF) { }

        if (!valida) {
            printf("Digite apenas letras de A a Z.\n");
            continue;
        }

        letra = (char)toupper((unsigned char)letra);
        const char *pal = palavra_por_letra(letra);

        if (repetida) {
            printf("Ops! Voce ja tentou a letra '%c'. Tente outra!\n", letra);
        } else if (ocorrencias > 0) {
            printf("✨ Voce acertou a letra %c de %s! Ela aparece %d vez(es). +%d pontos!\n",
                   letra, pal, ocorrencias, 10 * ocorrencias);
        } else {
            printf("😅 Poxa, a letra %c de %s nao é a correta, tente novamente! -5 pontos.\n",
                   letra, pal);
            printf("Restam %d tentativa(s).\n", jogo.tentativas_restantes);
        }

        // lembrete de tempo
        if (jogo.tempo_limite_seg > 0) {
            int t = tempo_restante(&jogo);
            if (t <= 10 && t > 0) {
                printf("Atencao: %d segundo(s) restantes!\n", t);
            }
        }
    }

    // estado final
    desenhar(&jogo);
    printf("\nObrigado por jogar!\n");
    return 0;
}
