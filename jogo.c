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
    // comando para limpar tela no windows
    system("cls");
#else
    // comando para limpar tela em linux / mac
    system("clear");
#endif
}

// funcao para pausar o programa ate o usuario apertar enter
void pausar() {
    printf("\nPressione ENTER para continuar...");
    getchar(); // espera o usuario apertar enter
}

// funcao para transformar uma string em letras maiusculas
static void strToUpper(char *s) {
    // percorre a string ate encontrar o fim '\0'
    while (*s) {
        // transforma o caractere atual em maiusculo
        *s = (char)toupper((unsigned char)*s);
        // avanca para o proximo caractere
        s++;
    }
}

// funcao para remover a quebra de linha no final de uma string lida com fgets
static void removeQuebraLinha(char *s) {
    // pega o tamanho atual da string
    size_t len = strlen(s);

    // se o tamanho for maior que zero e o ultimo caractere for \n ou \r, remove
    if (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
    }
}

/* =================== carregamento de dados =================== */

// funcao que carrega as palavras do arquivo para o vetor
int carregarPalavras(const char *nomeArquivo, Palavra vetor[], int *qtdTotal) {
    FILE *f;
    char linha[512];
    int contador;

    // tenta abrir o arquivo no modo leitura
    f = fopen(nomeArquivo, "r");
    contador = 0;

    // se nao conseguiu abrir, mostra erro e sai
    if (!f) {
        printf("Erro ao abrir o arquivo de palavras (%s).\n", nomeArquivo);
        return 0;
    }

    // le o arquivo linha por linha
    while (fgets(linha, sizeof(linha), f) != NULL && contador < MAX_PALAVRAS) {
        // se a linha estiver vazia, pula
        if (linha[0] == '\n' || linha[0] == '\0') {
            continue;
        }

        // cria uma variavel do tipo palavra
        Palavra p;
        char *token;

        // pega o tema (primeiro campo separado por ;)
        token = strtok(linha, ";\n\r");
        if (!token) {
            continue;
        }
        strncpy(p.tema, token, MAX_TEMA);
        p.tema[MAX_TEMA - 1] = '\0';

        // pega a palavra (segundo campo)
        token = strtok(NULL, ";\n\r");
        if (!token) {
            continue;
        }
        strncpy(p.palavra, token, MAX_PALAVRA);
        p.palavra[MAX_PALAVRA - 1] = '\0';

        // pega a dica (terceiro campo)
        token = strtok(NULL, ";\n\r");
        if (!token) {
            continue;
        }
        strncpy(p.dica, token, MAX_DICA);
        p.dica[MAX_DICA - 1] = '\0';

        // pega o nivel (quarto campo)
        token = strtok(NULL, ";\n\r");
        if (!token) {
            continue;
        }
        strncpy(p.nivel, token, MAX_NIVEL);
        p.nivel[MAX_NIVEL - 1] = '\0';

        // pega as letras visiveis (quinto campo)
        token = strtok(NULL, ";\n\r");
        if (!token) {
            continue;
        }
        p.letrasVisiveis = atoi(token);

        // pega o tempo maximo (sexto campo)
        token = strtok(NULL, ";\n\r");
        if (!token) {
            continue;
        }
        p.tempoMax = atoi(token);

        // pega o numero maximo de tentativas (setimo campo)
        token = strtok(NULL, ";\n\r");
        if (!token) {
            continue;
        }
        p.tentativasMax = atoi(token);

        // transforma alguns campos em maiusculo para padronizar
        strToUpper(p.tema);
        strToUpper(p.palavra);
        strToUpper(p.nivel);

        // coloca a palavra lida no vetor
        vetor[contador] = p;
        contador++;
    }

    // fecha o arquivo
    fclose(f);

    // atualiza a quantidade total de palavras carregadas
    *qtdTotal = contador;

    // se nenhuma palavra foi carregada, mostra aviso
    if (contador == 0) {
        printf("Nenhuma palavra foi carregada do arquivo.\n");
        return 0;
    }

    // se deu tudo certo, retorna 1
    return 1;
}

// funcao que filtra palavras por tema e coloca em outro vetor
int filtrarPorTema(const Palavra origem[], int qtdOrigem, const char *tema,
                   Palavra destino[], int *qtdDestino) {
    int i;
    int j;

    // inicia o indice do vetor destino em zero
    j = 0;

    // percorre todas as palavras de origem
    for (i = 0; i < qtdOrigem; i++) {
        // se o tema da palavra for igual ao tema escolhido, copia
        if (strcmp(origem[i].tema, tema) == 0) {
            destino[j] = origem[i];
            j++;
        }
    }

    // atualiza a quantidade de palavras filtradas
    *qtdDestino = j;

    // retorna 1 se encontrou pelo menos uma, senao 0
    if (j > 0) {
        return 1;
    } else {
        return 0;
    }
}

// funcao simples para sortear um indice com base em um limite
int sortearIndice(int limite) {
    // se o limite for menor ou igual a zero, nao faz sorteio
    if (limite <= 0) {
        return -1;
    }

    // retorna um numero aleatorio entre 0 e limite - 1
    return rand() % limite;
}

/* =================== interface de texto =================== */

// funcao para mostrar o titulo do jogo
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

// funcao para mostrar o menu principal
void exibirMenuPrincipal() {
    // mostra o titulo
    exibirTitulo();

    // mostra as opcoes do menu
    printf("Escolha um tema:\n\n");
    printf("  1) Princesas\n");
    printf("  2) Aventura\n");
    printf("  3) Animais\n");
    printf("  4) Como jogar\n");
    printf("  5) Ver ranking\n");
    printf("  0) Sair\n\n");
    printf("Opcao: ");
}

// funcao para mostrar as instrucoes de como jogar
void exibirComoJogar() {
    // limpa a tela antes
    limparTela();

    // mostra o titulo
    exibirTitulo();

    // mostra o texto de ajuda de como jogar
    printf("COMO JOGAR: \n");
    printf("- Escolha um tema (Princesas, Aventura ou Animais).\n");
    printf("- O jogo sorteia uma palavra secreta desse tema.\n");
    printf("- Voce ve uma dica e o numero de letras.\n");
    printf("- Digite uma letra por vez para tentar adivinhar.\n");
    printf("- Cada letra correta vale pontos, cada erro custa tentativas.\n");
    printf("- Ha um limite de tempo e de tentativas.\n");
    printf("- Se voce descobrir a palavra antes do tempo acabar, vence a partida.\n");

    // pausa para o jogador ler
    pausar();
}

/* =================== ranking =================== */

// funcao para registrar um resultado no arquivo de ranking
void registrarRanking(const char *nomeJogador, const Palavra *p, int pontuacao, int tempoGasto) {
    FILE *f;

    // abre o arquivo de ranking para acrescentar no final
    f = fopen(ARQUIVO_RANKING, "a");

    // se nao conseguiu abrir, mostra erro e sai
    if (!f) {
        printf("Nao foi possivel registrar ranking.\n");
        return;
    }

    // escreve uma linha com os dados do jogador
    fprintf(
        f,
        "%s;%s;%s;%d;%d\n",
        nomeJogador,
        p->tema,
        p->palavra,
        pontuacao,
        tempoGasto
    );

    // fecha o arquivo
    fclose(f);
}

// funcao para exibir o ranking do arquivo
void exibirRanking() {
    FILE *f;
    char linha[256];
    int pos;

    // limpa a tela e mostra o titulo
    limparTela();
    exibirTitulo();
    printf("RANKING\n\n");

    // abre o arquivo de ranking para leitura
    f = fopen(ARQUIVO_RANKING, "r");

    // se nao conseguiu abrir, nao ha ranking salvo ainda
    if (!f) {
        printf("Nenhum registro de ranking encontrado ainda.\n\n");
        pausar();
        return;
    }

    // cabecalho da tabela de ranking
    printf(" #  %-15s %-10s %-12s %-8s %-8s\n",
           "Nome", "Tema", "Palavra", "Pontos", "Tempo");
    printf("---------------------------------------------------------------\n");

    // posicao inicial do ranking
    pos = 1;

    // le o arquivo linha por linha
    while (fgets(linha, sizeof(linha), f) != NULL) {
        char *token;
        char nome[50];
        char tema[20];
        char palavra[32];
        int pontos;
        int tempo;

        // pega o nome
        token = strtok(linha, ";\n\r");
        if (!token) {
            continue;
        }
        strncpy(nome, token, sizeof(nome));
        nome[sizeof(nome) - 1] = '\0';

        // pega o tema
        token = strtok(NULL, ";\n\r");
        if (!token) {
            continue;
        }
        strncpy(tema, token, sizeof(tema));
        tema[sizeof(tema) - 1] = '\0';

        // pega a palavra
        token = strtok(NULL, ";\n\r");
        if (!token) {
            continue;
        }
        strncpy(palavra, token, sizeof(palavra));
        palavra[sizeof(palavra) - 1] = '\0';

        // pega os pontos
        token = strtok(NULL, ";\n\r");
        if (!token) {
            continue;
        }
        pontos = atoi(token);

        // pega o tempo
        token = strtok(NULL, ";\n\r");
        if (!token) {
            continue;
        }
        tempo = atoi(token);

        // imprime na tela a linha do ranking
        printf("%2d  %-15s %-10s %-12s %-8d %-8d\n",
               pos, nome, tema, palavra, pontos, tempo);

        // avanca para a proxima posicao
        pos++;
    }

    // fecha o arquivo
    fclose(f);

    printf("\n");
    pausar();
}

/* =================== logica do jogo =================== */

// funcao para mostrar letras ja usadas e letras dadas como dica
static void exibirLetrasUsadas(int usadas[26], int dicas[26]) {
    int i;
    int primeiro;

    // flag para saber se ja mostrou alguma letra
    primeiro = 1;

    printf("Letras ja mostradas: ");

    // percorre o vetor de a ate z (26 letras)
    for (i = 0; i < 26; i++) {
        // se a letra ja foi usada ou foi dica, mostra
        if (usadas[i] || dicas[i]) {
            // se nao for a primeira, coloca virgula antes
            if (!primeiro) {
                printf(", ");
            }
            printf("%c", 'A' + i);
            primeiro = 0;
        }
    }

    // se nenhuma letra foi mostrada ainda
    if (primeiro) {
        printf("nenhuma ainda");
    }
    printf("\n");
}

// funcao principal de uma partida do jogo
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

    // pega o tamanho da palavra
    tamanho = (int)strlen(p->palavra);

    // define tentativas, pontuacao inicial e letras visiveis
    tentativasRestantes = p->tentativasMax;
    pontuacao = 0;
    letrasRestantes = 0;
    letrasJaVisiveis = p->letrasVisiveis;

    // inicializa os vetores de letras usadas e dicas com zero
    for (i = 0; i < 26; i++) {
        usadas[i] = 0;
        dicas[i] = 0;
    }

    // prepara a exibicao: '_' para letras e ' ' para espaco
    for (i = 0; i < tamanho; i++) {
        if (p->palavra[i] == ' ') {
            exibicao[i] = ' ';
        } else {
            exibicao[i] = '_';
        }
    }
    exibicao[tamanho] = '\0';

    // revela as letras visiveis iniciais como dica
    if (letrasJaVisiveis > 0 && letrasJaVisiveis < tamanho) {
        int cont;

        // contador de letras ja reveladas
        cont = 0;

        // continua ate revelar a quantidade desejada
        while (cont < letrasJaVisiveis) {
            int idx;

            // sorteia uma posicao da palavra
            idx = rand() % tamanho;

            // se for uma letra ainda escondida e nao for espaco, revela
            if (exibicao[idx] == '_' && p->palavra[idx] != ' ') {
                exibicao[idx] = p->palavra[idx];
                cont++;

                // marca essa letra como dica (nao como usada)
                if (p->palavra[idx] >= 'A' && p->palavra[idx] <= 'Z') {
                    int li;
                    li = p->palavra[idx] - 'A';
                    dicas[li] = 1;
                }
            }
        }
    }

    // conta quantas letras ainda faltam ser descobertas
    letrasRestantes = 0;
    for (i = 0; i < tamanho; i++) {
        if (exibicao[i] == '_') {
            letrasRestantes++;
        }
    }

    // marca o inicio do tempo
    time(&inicio);

    // loop principal da partida: continua enquanto tiver tentativas e letras faltando
    while (tentativasRestantes > 0 && letrasRestantes > 0) {
        char letra;
        int acerto;

        // marca que ainda nao houve acerto nesta rodada
        acerto = 0;

        // pega o tempo atual
        time(&agora);
        tempoGasto = (int)difftime(agora, inicio);

        // verifica se estourou o tempo maximo
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

        // a cada rodada, limpa a tela e redesenha todas as informacoes
        limparTela();
        exibirTitulo();
        printf("TEMA: %s\n", p->tema);
        printf("Nivel: %s | Tempo maximo: %d s | Tentativas: %d\n\n",
               p->nivel, p->tempoMax, p->tentativasMax);
        printf("DICA: %s\n\n", p->dica);

        // mostra a palavra com letras e underlines
        printf("Palavra: ");
        for (i = 0; i < tamanho; i++) {
            printf("%c ", exibicao[i]);
        }
        printf("\n");

        // mostra tentativas restantes, pontuacao e tempo
        printf("Tentativas restantes: %d | Pontuacao: %d | Tempo: %ds\n",
               tentativasRestantes, pontuacao, tempoGasto);

        // mostra letras que ja foram usadas ou dadas como dica
        exibirLetrasUsadas(usadas, dicas);

        // pede uma letra para o jogador
        printf("Digite uma letra: ");

        // le a letra digitada
        if (scanf(" %c", &letra) != 1) {
            int ch;
            // descarta qualquer lixo do buffer
            ch = getchar();
            while (ch != '\n' && ch != EOF) {
                ch = getchar();
            }
            printf("Entrada invalida.\n");
            pausar();
            continue;
        }

        // limpa o resto da linha do buffer
        {
            int ch;
            ch = getchar();
            while (ch != '\n' && ch != EOF) {
                ch = getchar();
            }
        }

        // transforma a letra digitada em maiuscula
        letra = (char)toupper((unsigned char)letra);

        // verifica se a letra esta entre A e Z
        if (letra < 'A' || letra > 'Z') {
            printf("Digite apenas letras de A a Z.\n");
            pausar();
            continue;
        }

        // calcula o indice da letra no vetor (0 para A, 1 para B, etc.)
        {
            int indiceLetra;
            indiceLetra = letra - 'A';

            // se a letra ja foi tentada antes (apenas em usadas), bloqueia
            if (usadas[indiceLetra]) {
                printf("Voce ja tentou a letra '%c'.\n", letra);
                exibirLetrasUsadas(usadas, dicas);
                pausar();
                continue;
            }

            // marca que essa letra foi tentada
            usadas[indiceLetra] = 1;
        }

        // verifica se a letra existe na palavra, letra por letra
        for (i = 0; i < tamanho; i++) {
            if (p->palavra[i] == letra && exibicao[i] == '_') {
                // revela a letra
                exibicao[i] = letra;
                acerto = 1;
                // adiciona pontos
                pontuacao += 10;
                // diminui o numero de letras restantes
                letrasRestantes--;
            }
        }

        // se nao acertou nenhuma letra
        if (!acerto) {
            printf("\nEssa letra nao existe na palavra.\n");
            pontuacao -= 5;
            if (pontuacao < 0) {
                pontuacao = 0;
            }
            tentativasRestantes--;
            pausar();
        }
    }

    // calcula o tempo final gasto
    time(&agora);
    tempoGasto = (int)difftime(agora, inicio);

    // tela final com resultado
    limparTela();
    exibirTitulo();
    printf("TEMA: %s\n", p->tema);
    printf("Nivel: %s | Tempo maximo: %d s | Tentativas: %d\n\n",
           p->nivel, p->tempoMax, p->tentativasMax);
    printf("DICA: %s\n\n", p->dica);

    // se nao restam mais letras, o jogador acertou a palavra
    if (letrasRestantes == 0) {
        int bonusTentativas;
        int bonusTempo;

        // bonus por tentativas restantes
        bonusTentativas = tentativasRestantes * 2;

        // bonus por tempo restante (a cada 5 segundos sobra 1 ponto)
        bonusTempo = (p->tempoMax - tempoGasto) / 5;
        if (bonusTempo < 0) {
            bonusTempo = 0;
        }

        printf("PARABENS! Voce descobriu a palavra: %s\n\n", p->palavra);
        printf("Pontuacao base: %d\n", pontuacao);
        printf("Bonus por tentativas restantes: %d\n", bonusTentativas);
        printf("Bonus por tempo: %d\n", bonusTempo);

        // soma os bonus na pontuacao final
        pontuacao += bonusTentativas + bonusTempo;

        printf("Pontuacao final: %d | Tempo utilizado: %ds\n", pontuacao, tempoGasto);

        // registra o nome do jogador no ranking
        {
            char nome[50];

            printf("\nDigite seu nome para registrar no ranking (ou deixe vazio para pular): ");
            if (fgets(nome, sizeof(nome), stdin) != NULL) {
                // remove \n do final
                removeQuebraLinha(nome);

                // se o nome nao estiver vazio, salva no arquivo
                if (strlen(nome) > 0) {
                    registrarRanking(nome, p, pontuacao, tempoGasto);
                    printf("Registro salvo!\n");
                } else {
                    printf("Registro nao salvo.\n");
                }
            }
        }
    } else if (tentativasRestantes == 0) {
        // se as tentativas acabaram antes de descobrir a palavra
        printf("Suas tentativas acabaram.\n");
        printf("A palavra era: %s\n", p->palavra);
    } else {
        // se o tempo acabou antes de descobrir a palavra
        printf("O tempo acabou.\n");
        printf("A palavra era: %s\n", p->palavra);
    }

    // pausa no final da partida
    pausar();
}
