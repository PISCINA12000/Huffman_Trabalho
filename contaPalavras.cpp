#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "codigos_auxiliares/TAD_PilhaEnd.h"
#include "codigos_auxiliares/TAD_PilhaStrings.h"

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
                pop(&p, &arvoreHuf); //tira o endereco da pilha
                memset(codigo, '\0', sizeof(codigo)); //string reinicializada
                popStringPilha(&ps, codigo); //tira a string com o caminho ateh o no
                arvoreHuf = arvoreHuf->dir; //arvore vai para a direita
                TLCod = strlen(codigo); //o tl recebe o tamanho do caminho atual
                codigo[TLCod] = '1'; //caminho para a direita, ou seja, mais 1
                TLCod++; //tl incrementado
            }
        }
    }
}

int main()
{
    FILE *ptr = fopen("entrada2.txt", "r");
    FILE *ptrBi = fopen("tabela.dat","wb");
    Floresta *floresta, *auxFloresta;
    ArvoreHuf *arvoreHuf;
    Tabela tabela;
    LinhaTabela linha;
    char caracter, palavraAux[40];
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
        printf("%c", caracter); // printar na tela o caracter
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

    // rewind(ptr); //volto o ponteiro para o inicio do arquivo
    fclose(ptr);

    // exibicao da tabela de palavras e frequencias
    printf("\n\nPalavras:\n");
    for (int i = 0; i < tabela.TL; i++)
    {
        printf("%s - ", tabela.palavras[i]);
        printf("Frequencia: %d\n", tabela.freq[i]);
    }

    // criacao da FLORESTA
    floresta = NULL;
    for (int i = 0; i < tabela.TL; i++)
    {
        insereFlorestaOrd(&floresta, i + 1, tabela.freq[i]);
        tabela.simbolo[i] = i + 1;
    }
    // exibir a floresta formada
    printf("\n\nSimbolos e frequencias:\n");
    auxFloresta = floresta;
    while (auxFloresta)
    {
        printf("Simbolo: %d - ", auxFloresta->arvore->simbolo);
        printf("Frequencia: %d\n", auxFloresta->arvore->freq);
        auxFloresta = auxFloresta->prox;
    }

    // CRIAR A ARVORE DE HUFFMAN
    arvoreHuf = NULL;                       // inicializo a arvore
    criarArvoreHuff(&arvoreHuf, &floresta); // criacao efetiva da arvore
    printf("\n\nArvore Huffman\n");
    exibeArvore(arvoreHuf); // exibir na tela a arvore formada

    // CRIAR OS CODIGOS DE HUFFMAN
    criarCodigos(arvoreHuf, &tabela);
    // Exibir tabela
    printf("\n\nSimbolo\tPalavra\tFrequencia\tCodigo\n");
    for (int i = 0; i < tabela.TL; i++)
    {
        printf("%d\t", tabela.simbolo[i]);
        printf("%s\t", tabela.palavras[i]);
        printf("%d\t", tabela.freq[i]);
        printf("%s\n", tabela.codigo[i]);
    }

    //armazenar de 1 em 1 na struct "linha" e armazenar no arquivo binario
    for(int i=0; i<tabela.TL; i++)
    {
        memset(linha.codigo, '\0', sizeof(linha.codigo));
        memset(linha.palavra, '\0', sizeof(linha.palavra));
        strcpy(linha.palavra, tabela.palavras[i]);
        strcpy(linha.codigo, tabela.codigo[i]);
        linha.freq = tabela.freq[i];
        linha.simbolo = tabela.simbolo[i];
        fwrite(&linha,sizeof(linha),1,ptrBi);
    }
    fclose(ptrBi);
    ptrBi = fopen("tabela.dat", "rb");
    printf("\n\nLeitura do arquivo binario\n");
    fread(&linha,sizeof(linha),1,ptrBi);
    while(!feof(ptrBi))
    {
        printf("%d\t", linha.simbolo);
        printf("%s\t", linha.palavra);
        printf("%d\t", linha.freq);
        printf("%s\n", linha.codigo);
        fread(&linha,sizeof(linha),1,ptrBi);
    }
    fclose(ptrBi);

    return 0;
}