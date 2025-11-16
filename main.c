#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "jogo.h"

int main() {
    Palavra todasPalavras[MAX_PALAVRAS];
    int qtdTotal = 0;
    int opcao;
    int ok;

    // semente do random
    srand((unsigned int)time(NULL));

    // Carregar todas as palavras do arquivo
    ok = carregarPalavras(ARQUIVO_PALAVRAS, todasPalavras, &qtdTotal);
    if (!ok) {
        printf("Nao foi possivel iniciar o jogo sem palavras.\n");
        return 1;
    }

    do {
        limparTela();
        exibirMenuPrincipal();

        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n');
            opcao = -1;
        }
        while (getchar() != '\n'); // limpar buffer

        if (opcao == 1 || opcao == 2 || opcao == 3) {
            char temaEscolhido[MAX_TEMA];
            char nivelEscolhido[MAX_NIVEL];
            int opcaoNivel;

            if (opcao == 1) {
                strcpy(temaEscolhido, "PRINCESA");
            } else if (opcao == 2) {
                strcpy(temaEscolhido, "AVENTURA");
            } else {
                strcpy(temaEscolhido, "ANIMAL");
            }

            printf("\nEscolha a dificuldade:\n\n");
            printf("  1) Facil\n");
            printf("  2) Medio\n");
            printf("  3) Dificil\n");
            printf("  4) Qualquer\n\n");
            printf("Opcao: ");

            if (scanf("%d", &opcaoNivel) != 1) {
                while (getchar() != '\n');
                opcaoNivel = 4;
            }
            while (getchar() != '\n'); // limpar buffer

            int filtrarPorNivel = 1;
            if (opcaoNivel == 1) {
                strcpy(nivelEscolhido, "FACIL");
            } else if (opcaoNivel == 2) {
                strcpy(nivelEscolhido, "MEDIO");
            } else if (opcaoNivel == 3) {
                strcpy(nivelEscolhido, "DIFICIL");
            } else {
                filtrarPorNivel = 0; // qualquer nivel
            }

            Palavra filtradas[MAX_PALAVRAS];
            int qtdFiltradas = 0;

            // filtra por tema e, opcionalmente, por nivel
            for (int i = 0; i < qtdTotal; i++) {
                if (strcmp(todasPalavras[i].tema, temaEscolhido) == 0) {
                    if (!filtrarPorNivel || strcmp(todasPalavras[i].nivel, nivelEscolhido) == 0) {
                        filtradas[qtdFiltradas++] = todasPalavras[i];
                    }
                }
            }

            if (qtdFiltradas == 0) {
                printf("Nao ha palavras cadastradas para esse tema/dificuldade.\n");
                pausar();
                continue;
            }

            int idx = sortearIndice(qtdFiltradas);
            if (idx < 0) {
                printf("Erro ao sortear palavra.\n");
                pausar();
                continue;
            }

            jogarPartida(&filtradas[idx]);
        } else if (opcao == 4) {
            exibirComoJogar();
        } else if (opcao == 5) {
            exibirRanking();
        } else if (opcao == 0) {
            limparTela();
            printf("Encerrando o jogo. Ate logo!\n");
        } else {
            printf("Opcao invalida. Tente novamente.\n");
            pausar();
        }

    } while (opcao != 0);

    return 0;
}
