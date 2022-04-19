#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "messages.h"

typedef struct cadaBomba /* nó da bomba */
{
    int line;
    int column;
    int time;
    struct cadaBomba *proximo;
} cadaBomba;

typedef struct /* lista das bombas*/
{
    cadaBomba *firstBomb;
    cadaBomba *lastBomb;
} BombMaps;

void MenuInicial()
{
    puts(MSG_TITLE);
    puts("show                - show the mine map");
    puts("propagate <x> <y>   - explode bomb at <x> <y>");
    puts("log <x> <y>	      - explode bomb at <x> <y>");
    puts("plant <x> <y>       - place bomb at <x> <y>");
    puts("export <filename>   - save file with current map");
    puts("quit                - exit program");
    puts("sos                 - show menu");
    puts(MSG_ENDTITLE);
}

void matrizinicial(int nLinhas, int nColunas, char **matriz) /* limpa a matriz */
{
    int linha, coluna;
    for (linha = 0; linha < nLinhas; linha++)
    {
        for (coluna = 0; coluna < nColunas; coluna++)
        {
            matriz[linha][coluna] = '_';
        }
    }
}

void printmatriz(int nLinhas, int nColunas, char **matriz) /* função do show */
{
    int linha, coluna;
    for (linha = 0; linha < nLinhas; linha++)
    {
        for (coluna = 0; coluna < nColunas; coluna++)
        {
            printf("%c", matriz[linha][coluna]);
        }
        printf("\n");
    }
}

void addlist(BombMaps *list, int dimX, int dimY, int tempo) /* adiciona as bombas a lista */
{
    cadaBomba *bomba = (cadaBomba *)malloc(sizeof(cadaBomba)); /* arranjar memoria para uma bomba */
    cadaBomba *extra = list->firstBomb;                        /* ponteiro auxiliar, verificador de tempo */
    bomba->column = dimY;
    bomba->line = dimX;
    bomba->time = tempo;
    bomba->proximo = NULL;

    if (list->lastBomb != NULL) /* ver se o tail esta vazio */
    {
        /* verifica se a proxima bomba é ordena por tempos */
        for (extra = list->firstBomb; extra->proximo != NULL && bomba->time >= extra->proximo->time; extra = extra->proximo)
        {
        }
        if (extra->proximo == NULL) /* tail */
        {
            extra->proximo = bomba; /* adiciona bomba no fim */
        }
        else
        {
            bomba->proximo = extra->proximo; /* proximo do novo nó */
            extra->proximo = bomba;          /* liga a seta ao novo nó */
        }
        extra->proximo = bomba;
    }
    else
    {
        list->firstBomb = bomba; /* adiciona na primeira posição a bomba */
    }
    list->lastBomb = bomba; /* adiciona a bomba a ultima posição */
}

void voltaBomb(int nLinhas, int nColunas, char **matriz, int linhasMatriz, int ColunasMatriz, BombMaps *list, int tempo) /* função do log - bombas á volta da bomba */
{
    /* local por cima da bomba */
    if (nLinhas - 1 >= 0)
    {
        if (matriz[nLinhas - 1][nColunas] == '.')
        {
            addlist(list, nLinhas - 1, nColunas, tempo + 10);
            matriz[nLinhas - 1][nColunas] = '?';
        }
    }
    /* local pelo canto superior esquerdo da bomba */
    if (nLinhas - 1 >= 0 && nColunas - 1 >= 0)
    {
        if (matriz[nLinhas - 1][nColunas - 1] == '.')
        {
            addlist(list, nLinhas - 1, nColunas - 1, tempo + 11);
            matriz[nLinhas - 1][nColunas - 1] = '?';
        }
    }
    /* local pela esquerda da bomba */
    if (nColunas - 1 >= 0)
    {
        if (matriz[nLinhas][nColunas - 1] == '.')
        {
            addlist(list, nLinhas, nColunas - 1, tempo + 12);
            matriz[nLinhas][nColunas - 1] = '?';
        }
    }
    /* local pelo canto inferior esquerdo da bomba */
    if (nLinhas + 1 < linhasMatriz && nColunas - 1 >= 0)
    {
        if (matriz[nLinhas + 1][nColunas - 1] == '.')
        {
            addlist(list, nLinhas + 1, nColunas - 1, tempo + 13);
            matriz[nLinhas + 1][nColunas - 1] = '?';
        }
    }
    /* local por baixo da bomba */
    if (nLinhas + 1 < linhasMatriz)
    {
        if (matriz[nLinhas + 1][nColunas] == '.')
        {
            addlist(list, nLinhas + 1, nColunas, tempo + 14);
            matriz[nLinhas + 1][nColunas] = '?';
        }
    }
    /* local pelo cando inferior direito da bomba */
    if (nLinhas + 1 < linhasMatriz && nColunas + 1 < ColunasMatriz)
    {
        if (matriz[nLinhas + 1][nColunas + 1] == '.')
        {
            addlist(list, nLinhas + 1, nColunas + 1, tempo + 15);
            matriz[nLinhas + 1][nColunas + 1] = '?';
        }
    }
    /* local direita da bomba */
    if (nColunas + 1 < ColunasMatriz)
    {
        if (matriz[nLinhas][nColunas + 1] == '.')
        {
            addlist(list, nLinhas, nColunas + 1, tempo + 16);
            matriz[nLinhas][nColunas + 1] = '?';
        }
    }
    /* local pelo canto superior direito da bomba */
    if (nLinhas - 1 >= 0 && nColunas + 1 < ColunasMatriz)
    {
        if (matriz[nLinhas - 1][nColunas + 1] == '.')
        {
            addlist(list, nLinhas - 1, nColunas + 1, tempo + 17);
            matriz[nLinhas - 1][nColunas + 1] = '?';
        }
    }
}

void troca(cadaBomba *p1, cadaBomba *p2)
{
    int tempo = p1->time; /* guarda na variavel auxiliar */
    p1->time = p2->time;
    p2->time = tempo;
}

int main(int argc, char **agrv) /* argc - nome do ficheiro; argv - o que lê (ler atravez da linha de comandos) */
{
    char texto[100], filename[100];
    char **matriz;
    char variavel;
    char ficheiro[200];
    int firstLine = 1;
    int numeroLinha = 0;  /* substitui o 24 */
    int numeroColuna = 0; /* substitui o 24 */
    int nLinhas = 0;
    int nColunas = 0;
    int tempo = 0;
    int m;
    int sizeExportLines, sizeExportColumns;

    cadaBomba *percorre;
    BombMaps *list;
    FILE *exporteMapa, *exporteMapa2;

    exporteMapa = exporteMapa2 = NULL;
    if (argc < 2)
    {
        printf(MSG_FNAME "\n");
        exit(0);
    }

    exporteMapa = fopen(agrv[1], "r");
    if (exporteMapa == NULL)
    {
        printf(MSG_FILE_OPN "\n");
        exit(0);
    }
    else
    {
        firstLine = 1;             /* comeca sempre pela primeira linha */
        while (!feof(exporteMapa)) /* feof é a ultima linha do ficheiro */
        {
            fgets(ficheiro, 125, exporteMapa);                                    /* o fgets guarda numa variavel cada linha do ficheiro e depois pode fazer os sscanfs dessa linha quantas vezes quiser */
            if (ficheiro[0] == '#' || ficheiro[0] == '\r' || ficheiro[0] == '\n') /* ver se o primeiro caracter é um enter ou um comentario */
            {
                continue; /* voltar ao do while */
            }
            if (firstLine)
            {
                if (sscanf(ficheiro, "%d %d", &numeroLinha, &numeroColuna) == 2) /* sscanf f da o numero de atribuições que neste caso tem de ser 2 */
                {
                    sizeExportLines = numeroLinha;
                    sizeExportColumns = numeroColuna;
                    matriz = (char **)malloc(numeroLinha * sizeof(char *)); /* leitura das linhas / malloc - recebe o tamanho da matriz */
                    for (m = 0; m < numeroLinha; m++)
                    {
                        matriz[m] = (char *)malloc(numeroColuna * sizeof(char)); /* leitura das colunas */
                    }
                    matrizinicial(numeroLinha, numeroColuna, matriz);
                    firstLine = 0;
                }
                else
                {
                    printf(MSG_FILE_CRP "\n");
                    exit(0);
                }
            }
            else if (sscanf(ficheiro, "%c %d %d", &variavel, &nLinhas, &nColunas) == 3) /* sscanf f da o numero de atribuições que neste caso tem de ser 3 */
            {
                if (!(variavel == '*' || variavel == '.') || (nLinhas < 0 || nLinhas > numeroLinha || nColunas < 0 || nColunas > numeroColuna))
                {
                    printf(MSG_FILE_CRP "\n");
                    exit(0);
                }
                matriz[nLinhas][nColunas] = variavel;
            }
            else if (ficheiro[0] != '\r' || ficheiro[0] != '\n' || ficheiro[0] != '#')
            { /* se nao for um caracter e dois numeros e nenhum destes caracteres da erro ex: . 1 b */
                printf(MSG_FILE_CRP "\n");
                exit(0);
            }
        } /* Fim de ficheiro */
    }
    fclose(exporteMapa);

    MenuInicial();
    do
    {
        printf(MSG_PROMPT);
        scanf("%s", texto);

        if (strcmp(texto, "show") == 0) /* Apresentar o mapa no terminal */
        {
            printmatriz(numeroLinha, numeroColuna, matriz);
        }
        else if (strcmp(texto, "propagate") == 0) /* Explodir a bomba e explodir o que está a volta (FAZER COM STACK)*/
        {
            scanf("%d %d", &nLinhas, &nColunas);
            if (!((nLinhas >= 0 && nColunas >= 0) && (nLinhas <= numeroLinha && nColunas <= numeroColuna)))
            {
                printf(MSG_INVAL_CRD "\n");
            }
            else if ((matriz[nLinhas][nColunas] != '.') && (matriz[nLinhas][nColunas] != '*'))
            {
                printf(MSG_NOBOMB "\n");
                continue; /* voltar ao do while */
            }
            tempo = 0;
            list = (BombMaps *)malloc(sizeof(BombMaps));
            addlist(list, nLinhas, nColunas, tempo); /* adicionar a primeira bomba á lista */
            matriz[nLinhas][nColunas] = '?';
            voltaBomb(nLinhas, nColunas, matriz, numeroLinha, numeroColuna, list, tempo);
            percorre = list->firstBomb; /* percorre a bomba na lista */
            while (percorre != NULL)    /* percorre a lista */
            {
                voltaBomb(percorre->line, percorre->column, matriz, numeroLinha, numeroColuna, list, percorre->time); /* chama a funcão dos ifs */
                percorre = percorre->proximo;
            }
            percorre = list->firstBomb;
            while (percorre != list->lastBomb) /* percorre a lista */
            {
                matriz[percorre->line][percorre->column] = '.';
                percorre = percorre->proximo;
            }
            matriz[percorre->line][percorre->column] = '.';
            percorre = list->firstBomb;
            while (percorre != list->lastBomb) /* percorre a lista */
            {
                matriz[percorre->line][percorre->column] = '*';
                printmatriz(numeroLinha, numeroColuna, matriz);
                percorre = percorre->proximo;
            }
            matriz[percorre->line][percorre->column] = '*';
            printmatriz(numeroLinha, numeroColuna, matriz);
            percorre = list->firstBomb;
        }
        else if (strcmp(texto, "log") == 0) /* Explodir a bomba, explodir o que está a volta e mostrar no ecrã o tempo e a coordenada da bomba que explodiu */
        {
            scanf("%d %d", &nLinhas, &nColunas);
            if (!((nLinhas >= 0 && nColunas >= 0) && (nLinhas <= numeroLinha && nColunas <= numeroColuna)))
            {
                printf(MSG_INVAL_CRD "\n");
            }
            else if ((matriz[nLinhas][nColunas] != '.') && (matriz[nLinhas][nColunas] != '*'))
            {
                printf(MSG_NOBOMB "\n");
                continue; /* voltar ao do while */
            }
            tempo = 0;
            list = (BombMaps *)malloc(sizeof(BombMaps));
            addlist(list, nLinhas, nColunas, tempo); /* adicionar a primeira bomba á lista */
            matriz[nLinhas][nColunas] = '?';
            voltaBomb(nLinhas, nColunas, matriz, numeroLinha, numeroColuna, list, tempo);
            percorre = list->firstBomb; /* percorre a bomba na lista */
            while (percorre != NULL)    /* percorre a lista */
            {
                voltaBomb(percorre->line, percorre->column, matriz, numeroLinha, numeroColuna, list, percorre->time); /* chama a funcão dos ifs */
                percorre = percorre->proximo;
            }
            percorre = list->firstBomb;
            while (percorre != list->lastBomb) /* percorre a lista */
            {
                matriz[percorre->line][percorre->column] = '*'; /* muda de ? para * */
                percorre = percorre->proximo;                   /* passa para o seguinte */
            }
            matriz[percorre->line][percorre->column] = '*'; /* adiciona * ao tail (lastbomb) */
            percorre = list->firstBomb;
            while (percorre != list->lastBomb) /* percorre a lista */
            {
                printf("%d [%d, %d]\n", percorre->time, percorre->line, percorre->column); /* imprime a lista */
                percorre = percorre->proximo;
            }
            printf("%d [%d, %d]\n", percorre->time, percorre->line, percorre->column); /* imprime o tail */
        }
        else if (strcmp(texto, "plant") == 0)
        {
            scanf("%d %d", &nLinhas, &nColunas);
            if (!((nLinhas >= 0 && nColunas >= 0) && (nLinhas <= numeroLinha && nColunas <= numeroColuna)))
            {
                printf(MSG_INVAL_CRD "\n");
                continue; /* voltar ao do while */
            }
            if (matriz[nLinhas][nColunas] == '*' || matriz[nLinhas][nColunas] == '_')
            {
                matriz[nLinhas][nColunas] = '.';
            }
        }
        else if (strcmp(texto, "export") == 0)
        {
            FILE *exporteMapa2;
            scanf("%s", filename);
            exporteMapa2 = fopen(filename, "w");
            fprintf(exporteMapa2, "%d %d\n", sizeExportLines, sizeExportColumns); /* colocar o tamanho do mapa */
            for (nLinhas = 0; nLinhas < numeroLinha; nLinhas++)                   /* dois ciclos para percorrer o array do mapa */
            {
                for (nColunas = 0; nColunas < numeroColuna; nColunas++)
                {
                    if (matriz[nLinhas][nColunas] != '_')
                    {
                        fprintf(exporteMapa2, "%c %d %d\n", matriz[nLinhas][nColunas], nLinhas, nColunas);
                    }
                }
            }
            fclose(exporteMapa2);
        }
        else if (strcmp(texto, "sos") == 0) /* Apresenta de novo o menu com as opções */
        {
            MenuInicial();
        }
        else if (strcmp(texto, "quit") == 0)
        {
            exit(0);
        }
        else
        { /* se não for nenhuma das opcoes */
            printf(MSG_INVAL_CMD "\n");
            printf(MSG_PROMPT);
            scanf(" %s", texto); /* resolver teste faz coisas // scanf("%[^\n]s", texto); */
            continue;
        }
    } while (strcmp(texto, "quit")); /* O programa deverá simplesmente terminar */
    exit(0);
}