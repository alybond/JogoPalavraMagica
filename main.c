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
            Palavra filtradas[MAX_PALAVRAS];
            int qtdFiltradas = 0;

            if (opcao == 1) {
                strcpy(temaEscolhido, "PRINCESA");
            } else if (opcao == 2) {
                strcpy(temaEscolhido, "AVENTURA");
            } else {
                strcpy(temaEscolhido, "ANIMAL");
            }

            if (!filtrarPorTema(todasPalavras, qtdTotal, temaEscolhido, filtradas, &qtdFiltradas)) {
                printf("Nao ha palavras cadastradas para o tema %s.\n", temaEscolhido);
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
