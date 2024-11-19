#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "codigos_auxiliares/TAD_PilhaEnd.h"
#include "codigos_auxiliares/TAD_PilhaStrings.h"

// STRUCTS -------------------------------------------------------------------
struct bits
{
    unsigned char b7 : 1; // - significativo
    unsigned char b6 : 1;
    unsigned char b5 : 1;
    unsigned char b4 : 1;
    unsigned char b3 : 1;
    unsigned char b2 : 1;
    unsigned char b1 : 1;
    unsigned char b0 : 1; // + significativo
};
union byte
{
    struct bits bi;
    unsigned char num;
};
typedef union byte Byte;

struct tabela
{
    char palavras[100][40], codigo[100][40];
    int freq[100], simbolo[100], TL;
    // TL = quantidade de palavras armazenadas
};
typedef struct tabela Tabela;

struct linhaTabela
{
    char palavra[40], codigo[40];
    int freq, simbolo;
};
typedef struct linhaTabela LinhaTabela;

struct floresta
{
    ArvoreHuf *arvore;
    struct floresta *prox;
};
typedef struct floresta Floresta;

// FUNCOES -------------------------------------------------------------------
// FLORESTA
Floresta *criaNoFloresta(int simbolo, int freq)
{
    Floresta *novo = (Floresta *)malloc(sizeof(Floresta));
    novo->arvore = (ArvoreHuf *)malloc(sizeof(ArvoreHuf));
    novo->arvore->dir = NULL;
    novo->arvore->esq = NULL;
    novo->arvore->freq = freq;
    novo->arvore->simbolo = simbolo;
    novo->arvore->soma = 0; // eh uma folha
    novo->prox = NULL;

    return novo;
}
Floresta *criaNoFlorestaSoma(int freq1, int freq2)
{
    Floresta *novo = (Floresta *)malloc(sizeof(Floresta));
    novo->arvore = (ArvoreHuf *)malloc(sizeof(ArvoreHuf));
    novo->arvore->dir = NULL;
    novo->arvore->esq = NULL;
    novo->arvore->freq = 0;
    novo->arvore->simbolo = 0;
    novo->arvore->soma = freq1 + freq2; // nao eh uma folha
    novo->prox = NULL;

    return novo;
}
void insereFlorestaOrd(Floresta **floresta, int simbolo, int freq)
{
    Floresta *no, *atual, *ant;
    atual = *floresta;
    if (*floresta) // possui algo na floresta
    {
        no = criaNoFloresta(simbolo, freq);
        ant = atual;
        if (no->arvore->freq < atual->arvore->freq) // posicao correta eh a primeira
        {
            no->prox = *floresta;
            *floresta = no;
        }
        else
        {
            while (atual != NULL && no->arvore->freq > atual->arvore->freq)
            {
                ant = atual;
                atual = atual->prox;
            }
            if (atual != NULL) // a posicao correta esta no meio da lista
            {
                no->prox = ant->prox;
                ant->prox = no;
            }
            else // a posicao correta esta ao final de todos
            {
                ant->prox = no;
            }
        }
    }
    else // floresta está vazia
    {
        *floresta = criaNoFloresta(simbolo, freq);
    }
}
void insereFlorestaOrdSoma(Floresta **floresta, Floresta *no)
{
    Floresta *atual, *ant;

    atual = *floresta;
    ant = atual;
    if (*floresta) // possui algo na lista
    {
        if (no->arvore->soma < atual->arvore->soma && no->arvore->soma < atual->arvore->freq) // posicao correta eh a primeira
        {
            no->prox = *floresta;
            *floresta = no;
        }
        else
        {
            while (atual != NULL && (no->arvore->soma > atual->arvore->soma || no->arvore->soma > atual->arvore->freq))
            {
                ant = atual;
                atual = atual->prox;
            }
            if (atual != NULL) // a posicao correta esta no meio da lista
            {
                no->prox = ant->prox;
                ant->prox = no;
            }
            else // a posicao correta esta ao final de todos
            {
                ant->prox = no;
            }
        }
    }
    else
    {
        *floresta = no;
    }
}

// ARVORE DE HUFFMAN
void criarArvoreHuff(ArvoreHuf **arvoreHuf, Floresta **floresta)
{
    Floresta *novo, *aux, *deletar;
    int freq1, freq2;

    while ((*floresta)->prox) // enquanto houver dois ou mais nodos na floresta
    {
        if ((*floresta)->arvore->soma > 0) // nao eh folha
        {
            freq1 = (*floresta)->arvore->soma;
        }
        else // eh folha
            freq1 = (*floresta)->arvore->freq;
        if ((*floresta)->prox->arvore->soma > 0) // nao ah folha
        {
            freq2 = (*floresta)->prox->arvore->soma;
        }
        else // eh folha
            freq2 = (*floresta)->prox->arvore->freq;

        novo = criaNoFlorestaSoma(freq1, freq2);
        novo->arvore->esq = (*floresta)->arvore;
        novo->arvore->dir = (*floresta)->prox->arvore;
        aux = *floresta;
        *floresta = (*floresta)->prox->prox;
        for (int i = 0; i < 2; i++)
        {
            deletar = aux;
            aux = aux->prox;
            free(deletar);
        }
        insereFlorestaOrdSoma(floresta, novo);
    }
    *arvoreHuf = (*floresta)->arvore;
    free(*floresta);
    *floresta = NULL;
}
void exibeArvore(ArvoreHuf *arvore)
{
    static int n = -1;
    if (arvore == NULL)
        return;
    n++;
    exibeArvore(arvore->dir);
    for (int i = 0; i < 5 * n; i++)
        printf(" ");
    if (arvore->soma == 0) // eh uma folha
    {
        printf("(%d, %d)\n", arvore->simbolo, arvore->freq);
    }
    else // nao eh uma folha
    {
        printf("( %d )\n", arvore->soma);
    }
    exibeArvore(arvore->esq);
    n--;
}

// CODIGOS HUFFMAN
void criarCodigos(ArvoreHuf *arvoreHuf, Tabela *tabela)
{
    Pilha *p;
    PilhaString *ps;
    char codigo[40];
    int TLCod = 0;

    init(&p);
    initStringPilha(&ps);
    memset(codigo, '\0', sizeof(codigo));

    if (arvoreHuf)
    {
        while (arvoreHuf || !isEmpty(p))
        {
            while (arvoreHuf)
            {
                // printf("%d ",arvoreHuf->info);
                // verificar se eh uma folha, se sim eu adiciono o codigo na minha tabela
                if (arvoreHuf->dir == NULL && arvoreHuf->esq == NULL) // eh uma folha
                {
                    // adiciono o codigo com o respectivo simbolo na minha tabela
                    int pos = 0;
                    while (arvoreHuf->simbolo > (*tabela).simbolo[pos])
                        pos++;
                    strcpy((*tabela).codigo[pos], codigo);
                }
                push(&p, arvoreHuf);
                pushStringPilha(&ps, codigo);
                arvoreHuf = arvoreHuf->esq;
                codigo[TLCod] = '0';
                TLCod++;
            }
            if (!isEmpty(p))
            {
                pop(&p, &arvoreHuf);                  // tira o endereco da pilha
                memset(codigo, '\0', sizeof(codigo)); // string reinicializada
                popStringPilha(&ps, codigo);          // tira a string com o caminho ateh o no
                arvoreHuf = arvoreHuf->dir;           // arvore vai para a direita
                TLCod = strlen(codigo);               // o tl recebe o tamanho do caminho atual
                codigo[TLCod] = '1';                  // caminho para a direita, ou seja, mais 1
                TLCod++;                              // tl incrementado
            }
        }
    }
}

// ARQUIVOS BINARIOS
void armazenarFraseCodificada(char *fraseCodificada)
{
    int i = 0;
    Byte u;
    FILE *Ptr = fopen("frase.dat", "wb+");
    if (Ptr == NULL)
        printf("Erro de Abertura");
    else
    {
        while (fraseCodificada[i] != '\0')
        {
            u.bi.b0 = fraseCodificada[i] - '0';
            u.bi.b1 = fraseCodificada[i + 1] - '0';
            u.bi.b2 = fraseCodificada[i + 2] - '0';
            u.bi.b3 = fraseCodificada[i + 3] - '0';
            u.bi.b4 = fraseCodificada[i + 4] - '0';
            u.bi.b5 = fraseCodificada[i + 5] - '0';
            u.bi.b6 = fraseCodificada[i + 6] - '0';
            u.bi.b7 = fraseCodificada[i + 7] - '0';
            i = i + 8;
            fwrite(&u.num, sizeof(char), 1, Ptr);
        }
    }

    fclose(Ptr);
}

// INT MAIN ------------------------------------------------------------------
int main()
{
    FILE *ptr = fopen("entrada3.txt", "r");
    FILE *ptrBi = fopen("tabela.dat", "wb");
    FILE *ptrBits = fopen("numeroBits.dat","wb");
    Floresta *floresta, *auxFloresta;
    ArvoreHuf *arvoreHuf;
    Tabela tabela;
    LinhaTabela linha;
    char caracter, palavraAux[40], fraseInteira[4000], fraseCodificada[500];
    int TLAux = 0;

    // inicializacao da minha tabela
    memset(palavraAux, '\0', sizeof(palavraAux)); // inicializar a string auxiliar
    for (int i = 0; i < 100; i++)                 // inicializar todas as strings da minha tabela
    {
        memset(tabela.palavras[i], '\0', sizeof(tabela.palavras[i])); // inicializando pos por pos as palavras
        memset(tabela.codigo[i], '\0', sizeof(tabela.codigo[i]));     // inicializando pos por pos os codigos
        tabela.freq[i] = 0;                                           // inicializar as frequencias
        tabela.simbolo[i] = 0;                                        // inicializar o TL de palavras
    }
    tabela.TL = 0;

    // le o arquivo texto inteiro que eh passado para o ponteiro
    caracter = fgetc(ptr);
    while (!feof(ptr))
    {
        // leitura do caracter
        //printf("%c", caracter); // printar na tela o caracter
        if (caracter == ' ')    // se o caracter lido for igual a espaço
        {
            // preciso buscar se jah existe o espaco (' ')
            int pos = 0;
            while (pos < tabela.TL && strcmp(" ", tabela.palavras[pos]) != 0)
                pos++;
            if (pos < tabela.TL) // ja possuo espaco na minha tabela
            {
                // se ja existe eu apenas incremento
                tabela.freq[pos]++;
            }
            else // se nao existe eu adiciono
            {
                strcpy(tabela.palavras[tabela.TL], " ");
                tabela.freq[pos]++;
                tabela.TL++;
            }

            // preciso buscar se jah existe a string na minha palavraAux
            pos = 0;
            while (pos < tabela.TL && strcmp(palavraAux, tabela.palavras[pos]) != 0)
                pos++;
            if (pos < tabela.TL) // ja possuo espaco na minha tabela
            {
                // se ja existe eu apenas incremento
                tabela.freq[pos]++;
            }
            else // se nao existe eu adiciono
            {
                strcpy(tabela.palavras[tabela.TL], palavraAux);
                tabela.freq[pos]++;
                tabela.TL++;
            }

            memset(palavraAux, '\0', sizeof(palavraAux)); // reinicio a minha string auxiliar
            TLAux = 0;                                    // comecar denovo a leitura dos caraceres
        }
        else // quer dizer que estou lendo um caracter valido
        {
            palavraAux[TLAux] = caracter;
            TLAux++;
        }
        caracter = fgetc(ptr);
    }
    if (palavraAux[0] != '\0')
    {
        int pos = 0;
        while (pos < tabela.TL && strcmp(palavraAux, tabela.palavras[pos]) != 0)
            pos++;
        if (pos < tabela.TL) // ja possuo espaco na minha tabela
        {
            // se ja existe eu apenas incremento
            tabela.freq[pos]++;
        }
        else // se nao existe eu adiciono
        {
            strcpy(tabela.palavras[tabela.TL], palavraAux);
            tabela.freq[pos]++;
            tabela.TL++;
        }
    }
    fclose(ptr);

    // exibicao da tabela de palavras e frequencias
    // printf("\n\nPalavras:\n");
    // for (int i = 0; i < tabela.TL; i++)
    // {
    //     printf("%s - ", tabela.palavras[i]);
    //     printf("Frequencia: %d\n", tabela.freq[i]);
    // }

    // CRIACAO DA FLORESTA
    floresta = NULL;
    for (int i = 0; i < tabela.TL; i++)
    {
        insereFlorestaOrd(&floresta, i + 1, tabela.freq[i]);
        tabela.simbolo[i] = i + 1;
    }
    // // exibir a floresta formada
    // printf("\n\nSimbolos e frequencias:\n");
    // auxFloresta = floresta;
    // while (auxFloresta)
    // {
    //     printf("Simbolo: %d - ", auxFloresta->arvore->simbolo);
    //     printf("Frequencia: %d\n", auxFloresta->arvore->freq);
    //     auxFloresta = auxFloresta->prox;
    // }

    // CRIAR A ARVORE DE HUFFMAN
    arvoreHuf = NULL;                       // inicializo a arvore
    criarArvoreHuff(&arvoreHuf, &floresta); // criacao efetiva da arvore
    printf("\n\nArvore Huffman\n");
    exibeArvore(arvoreHuf); // exibir na tela a arvore formada

    // CRIAR OS CODIGOS DE HUFFMAN
    criarCodigos(arvoreHuf, &tabela);
    // Exibir tabela
    // printf("\n\nSimbolo\tPalavra\tFrequencia\tCodigo\n");
    // for (int i = 0; i < tabela.TL; i++)
    // {
    //     printf("%d\t", tabela.simbolo[i]);
    //     printf("%s\t", tabela.palavras[i]);
    //     printf("%d\t", tabela.freq[i]);
    //     printf("%s\n", tabela.codigo[i]);
    // }

    // CRIANDO A TABELA NO ARQUIVO BINARIO
    for (int i = 0; i < tabela.TL; i++)
    {
        memset(linha.codigo, '\0', sizeof(linha.codigo));
        memset(linha.palavra, '\0', sizeof(linha.palavra));
        strcpy(linha.palavra, tabela.palavras[i]);
        strcpy(linha.codigo, tabela.codigo[i]);
        linha.freq = tabela.freq[i];
        linha.simbolo = tabela.simbolo[i];
        fwrite(&linha, sizeof(linha), 1, ptrBi);
    }
    fclose(ptrBi);

    // LEITURA DO ARQUIVO BINARIO
    ptrBi = fopen("tabela.dat", "rb");
    printf("\n\nLeitura do arquivo binario\n");
    fread(&linha, sizeof(linha), 1, ptrBi);
    while (!feof(ptrBi))
    {
        printf("%d\t", linha.simbolo);
        printf("%s\t", linha.palavra);
        printf("%d\t", linha.freq);
        printf("%s\n", linha.codigo);
        fread(&linha, sizeof(linha), 1, ptrBi);
    }
    fclose(ptrBi);

    // SE MUDAR O ARQUIVO LOGO APOS O INT MAIN, TERA QUE MUDAR O ARQUIVO DE ENTRADA LOGO ABAIXO TAMBEM
    // ARMAZENAR A ENTRADA EM UMA STRING PARA DEPOIS CODIFICA-LA COM A ARVORE E A TABELA
    ptr = fopen("entrada3.txt", "r");
    memset(fraseInteira, '\0', sizeof(fraseInteira));
    fraseInteira[strlen(fraseInteira)] = fgetc(ptr);
    while (!feof(ptr))
    {
        fraseInteira[strlen(fraseInteira)] = fgetc(ptr);
    }
    fraseInteira[strlen(fraseInteira) - 1] = '\0'; // RETIRA O EOF AO FINAL DA STRING
    //printf("%s\n", fraseInteira);
    fclose(ptr);

    // CODIFICANDO A ENTRADA INTEIRA PARA DEPOIS ARMAZENAR O CODIGO
    memset(palavraAux, '\0', sizeof(palavraAux));
    memset(fraseCodificada, '\0', sizeof(fraseCodificada));
    TLAux = 0;
    int TLCod = 0;
    for (int i = 0; i < strlen(fraseInteira); i++)
    {
        palavraAux[TLAux] = fraseInteira[i];
        TLAux++;
        if (palavraAux[TLAux - 1] == ' ') // se o caracter lido for igual a espaço
        {
            // buscar a respectiva palavra
            palavraAux[TLAux - 1] = '\0';
            TLAux--;
            int pos = 0;
            while (pos < tabela.TL && strcmp(palavraAux, tabela.palavras[pos]) != 0)
                pos++;
            if (pos < tabela.TL) // achei na minha tabela a palavra
            {
                for (int j = 0; j < strlen(tabela.codigo[pos]); j++)
                {
                    fraseCodificada[TLCod] = tabela.codigo[pos][j];
                    TLCod++;
                }
            }
            else
            {
                printf("Erro a palavra '%s' nao existe\n", tabela.palavras[pos]);
            }
            memset(palavraAux, '\0', sizeof(palavraAux)); // reinicio a minha string auxiliar
            TLAux = 0;

            pos = 0;
            while (pos < tabela.TL && strcmp(" ", tabela.palavras[pos]) != 0)
                pos++;
            if (pos < tabela.TL) // achei na minha tabela a palavra
            {
                for (int j = 0; j < strlen(tabela.codigo[pos]); j++)
                {
                    fraseCodificada[TLCod] = tabela.codigo[pos][j];
                    TLCod++;
                }
            }
            else // nao existe na tabela
            {
                printf("Erro, palavra '%s' nao existe\n", tabela.palavras[pos]);
            }
        }
    }
    if (palavraAux[0] != '\0')
    {
        int pos = 0;
        while (pos < tabela.TL && strcmp(palavraAux, tabela.palavras[pos]) != 0)
            pos++;
        if (pos < tabela.TL) // achei a palavra
        {
            for (int j = 0; j < strlen(tabela.codigo[pos]); j++)
            {
                fraseCodificada[TLCod] = tabela.codigo[pos][j];
                TLCod++;
            }
        }
        else
        {
            printf("Erro a palavra '%s' nao existe\n", tabela.palavras[pos]);
        }
    }
    // EXIBICAO DA FRASE CODIFICADA
    // printf("\n\nFrase codificada:\n");
    // printf("%s\n", fraseCodificada);
    // printf("\n\nBytes inteiros na frase codificada: %ld\n", strlen(fraseCodificada) / 8);
    // printf("Numero de bits faltantes: %ld\n", 8 - strlen(fraseCodificada) % 8);

    // FAZ A CALCULO PARA DESCOBRIR QUANTOS ZEROS COLOCAR AO FINAL DA STRING CODIFICADA
    int restantes = 8 - strlen(fraseCodificada) % 8;
    if (restantes < 8) // SE MENOR QUE 8 ENTAO A FRASE CODIFICADA NAO EH MULTIPLA DE 8
    {
        for (int i = 0; i < restantes; i++)
        {
            fraseCodificada[strlen(fraseCodificada)] = '0';
        }
        fwrite(&restantes,sizeof(int),1,ptrBits);
    }
    else // SE FOR IGUAL A 8 QUER DIZER QUE EH MULTIPLA DE 8. ESTAH CORRETO
    {
        restantes = 0;
        fwrite(&restantes,sizeof(int),1,ptrBits);
    }

    // EXIBICAO DA FRASE CODIFICADA COM OS DEVIDOS ZEROS
    printf("\n\nFrase codificada:\n");
    printf("%s\n", fraseCodificada);

    // ARMAZENAMENTO DA FRASE CODIFICADA EM UM ARQUIVO BINARIO
    armazenarFraseCodificada(fraseCodificada);
    
    // LEITURA DO ARQUIVO QUE CONTEM A QUANTIDADE DE ZEROS ADICIONADA A STRIG CODIFICADA
    // printf("\n\nBits a mais:\n");
    // fseek(ptrBits,0,0);
    // fread(&restantes,sizeof(int),1,ptrBits);
    // printf("%d\n",restantes);
    fcloseall();

    return 0;
}