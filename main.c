#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "jogo.h"

// programa principal do jogo
int main() {
    // vetor com todas as palavras carregadas do arquivo
    Palavra todasPalavras[MAX_PALAVRAS];

    // quantidade total de palavras carregadas
    int qtdTotal = 0;

    // opcao escolhida no menu principal
    int opcao;

    // variavel para saber se o carregamento deu certo
    int ok;

    // semente do numero aleatorio baseada no horario atual
    // isso faz o sorteio mudar cada vez que o jogo abre
    srand((unsigned int)time(NULL));

    // carrega todas as palavras do arquivo de texto
    ok = carregarPalavras(ARQUIVO_PALAVRAS, todasPalavras, &qtdTotal);

    // se nao conseguiu carregar, nao tem como jogar
    if (!ok) {
        printf("Nao foi possivel iniciar o jogo sem palavras.\n");
        return 1; // encerra o programa com erro
    }

    // loop principal do jogo (menu)
    do {
        // limpa a tela antes de mostrar o menu
        limparTela();

        // mostra o menu principal
        exibirMenuPrincipal();

        // tenta ler a opcao escolhida
        if (scanf("%d", &opcao) != 1) {
            // se deu erro ao ler, limpa o buffer e coloca uma opcao invalida
            while (getchar() != '\n');
            opcao = -1;
        }

        // limpa o buffer de entrada (remove resto da linha)
        while (getchar() != '\n');

        // se a opcao for um tema (1, 2 ou 3), inicia uma partida
        if (opcao == 1 || opcao == 2 || opcao == 3) {
            // texto do tema escolhido
            char temaEscolhido[MAX_TEMA];

            // texto do nivel escolhido
            char nivelEscolhido[MAX_NIVEL];

            // opcao de dificuldade
            int opcaoNivel;

            // define o tema com base na opcao
            if (opcao == 1) {
                // tema princesas
                strcpy(temaEscolhido, "PRINCESA");
            } else if (opcao == 2) {
                // tema aventura
                strcpy(temaEscolhido, "AVENTURA");
            } else {
                // tema animais
                strcpy(temaEscolhido, "ANIMAL");
            }

            // menu de dificuldade
            printf("\nEscolha a dificuldade:\n\n");
            printf("  1) Facil\n");
            printf("  2) Medio\n");
            printf("  3) Dificil\n");
            printf("  4) Qualquer\n\n");
            printf("Opcao: ");

            // tenta ler a opcao de dificuldade
            if (scanf("%d", &opcaoNivel) != 1) {
                // se der erro, limpa o buffer e coloca como "qualquer"
                while (getchar() != '\n');
                opcaoNivel = 4;
            }

            // limpa o buffer da entrada
            while (getchar() != '\n');

            // flag para saber se vai filtrar por nivel ou nao
            int filtrarPorNivel = 1;

            // define o nivel de acordo com a escolha
            if (opcaoNivel == 1) {
                strcpy(nivelEscolhido, "FACIL");
            } else if (opcaoNivel == 2) {
                strcpy(nivelEscolhido, "MEDIO");
            } else if (opcaoNivel == 3) {
                strcpy(nivelEscolhido, "DIFICIL");
            } else {
                // se for 4 ou algo invalido, nao filtra por nivel
                filtrarPorNivel = 0;
            }

            // vetor que guarda apenas as palavras filtradas pelo tema (e nivel)
            Palavra filtradas[MAX_PALAVRAS];

            // quantidade de palavras encontradas apos o filtro
            int qtdFiltradas = 0;

            // indice para percorrer o vetor de todas as palavras
            int i;

            // filtra por tema e, se necessario, por nivel
           
            for (i = 0; i < qtdTotal; i++) {
                // verifica se o tema da palavra e igual ao tema escolhido
                if (strcmp(todasPalavras[i].tema, temaEscolhido) == 0) {
                    // se nao precisa filtrar nivel, ja aceita
                    // se precisa filtrar, compara o nivel
                    if (!filtrarPorNivel || strcmp(todasPalavras[i].nivel, nivelEscolhido) == 0) {
                        // copia a palavra para o vetor filtrado
                        filtradas[qtdFiltradas] = todasPalavras[i];
                        // aumenta o contador de filtradas
                        qtdFiltradas++;
                    }
                }
            }

            // se nenhuma palavra foi encontrada para esse tema/dificuldade
            if (qtdFiltradas == 0) {
                printf("Nao ha palavras cadastradas para esse tema/dificuldade.\n");
                pausar();
                // volta para o menu
                continue;
            }

            // sorteia um indice dentro do vetor de palavras filtradas
            int idx;
            idx = sortearIndice(qtdFiltradas);

            // se deu algum problema no sorteio
            if (idx < 0) {
                printf("Erro ao sortear palavra.\n");
                pausar();
                continue;
            }

            // inicia uma partida com a palavra sorteada
            jogarPartida(&filtradas[idx]);
        } else if (opcao == 4) {
            // mostra a tela de "como jogar"
            exibirComoJogar();
        } else if (opcao == 5) {
            // mostra o ranking
            exibirRanking();
        } else if (opcao == 0) {
            // encerra o jogo
            limparTela();
            printf("Encerrando o jogo. Ate logo!\n");
        } else {
            // qualquer outra opcao e invalida
            printf("Opcao invalida. Tente novamente.\n");
            pausar();
        }

    // o loop continua enquanto a opcao for diferente de 0
    } while (opcao != 0);

    // fim normal do programa
    return 0;
}
