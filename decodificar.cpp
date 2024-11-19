#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// STRUCTS-----------------------------------------------------------------------
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

struct arvore
{
    int simbolo; // quando a soma for zero então sei que é uma folha
    struct arvore *esq;
    struct arvore *dir;
};
typedef struct arvore Arvore;

struct linhaTabela
{
    char palavra[40], codigo[40];
    int freq, simbolo;
};
typedef struct linhaTabela LinhaTabela;

struct tabela
{
    LinhaTabela info;
    struct tabela *prox;
};
typedef struct tabela Tabela;

// FUNCOES ----------------------------------------------------------------------
// ARQUIVO BINARIO***************************************************************
void lerArqFraseCodificada(char *fraseCodificada)
{
    FILE *Ptr = fopen("frase.dat", "rb");
    if (Ptr == NULL)
        printf("Erro de Abertura");
    else
    {
        Byte u;
        int i = 0;
        rewind(Ptr);
        fread(&u.num, sizeof(char), 1, Ptr);
        while (!feof(Ptr))
        {

            fraseCodificada[i] = u.bi.b0 + '0';
            fraseCodificada[i + 1] = u.bi.b1 + '0';
            fraseCodificada[i + 2] = u.bi.b2 + '0';
            fraseCodificada[i + 3] = u.bi.b3 + '0';
            fraseCodificada[i + 4] = u.bi.b4 + '0';
            fraseCodificada[i + 5] = u.bi.b5 + '0';
            fraseCodificada[i + 6] = u.bi.b6 + '0';
            fraseCodificada[i + 7] = u.bi.b7 + '0';

            i = i + 8;

            fread(&u.num, sizeof(char), 1, Ptr);
        }
        fraseCodificada[i] = '\0';
    }
    fclose(Ptr);
}
// ARVORE HUFFMAN****************************************************************
Arvore *criaNo(int simbolo)
{
    Arvore *NC;
    NC = (Arvore *)malloc(sizeof(Arvore));
    NC->dir = NULL;
    NC->esq = NULL;
    NC->simbolo = simbolo;
    return NC;
}
void construirArvoreBB(Arvore **raiz)
{
    FILE *Ptr = fopen("tabela.dat", "rb");
    if (Ptr == NULL)
        printf("Erro de Abertura");
    else
    {
        LinhaTabela Reg;
        rewind(Ptr);
        fread(&Reg, sizeof(LinhaTabela), 1, Ptr);
        while (!feof(Ptr))
        {
            Arvore *NC, *aux;
            char codigo[40];
            int i = 0;

            codigo[0] = '\0';
            strcpy(codigo, Reg.codigo);
            if (*raiz == NULL) // inicializa
                *raiz = criaNo(-1);
            aux = *raiz;
            while (codigo[i] != '\0')
            {
                if (codigo[i] == '0')
                {
                    if (aux->esq == NULL)
                    {
                        aux->esq = criaNo(-1);
                        aux = aux->esq;
                    }
                    else
                    {
                        aux = aux->esq;
                    }
                }
                else if (aux->dir == NULL)
                {
                    aux->dir = criaNo(-1);
                    aux = aux->dir;
                }
                else
                {
                    aux = aux->dir;
                }
                i++;
            }
            aux->simbolo = Reg.simbolo;
            fread(&Reg, sizeof(LinhaTabela), 1, Ptr);
        }
    }
    fclose(Ptr);
}
void exibeArvore(Arvore *arvore)
{
    static int n = -1;
    if (arvore == NULL)
        return;
    n++;
    exibeArvore(arvore->dir);
    for (int i = 0; i < 5 * n; i++)
        printf(" ");
    printf("( %d )\n",arvore->simbolo);
    exibeArvore(arvore->esq);
    n--;
}
// LISTA ENCADEADA***************************************************************
Tabela* criaNoTabela(LinhaTabela linha)
{
    Tabela *nova = (Tabela*)malloc(sizeof(Tabela));
    nova->info = linha;
    nova->prox = NULL;

    return nova;
}
void inserirTabela(Tabela **tabela, LinhaTabela linha)
{
    Tabela *atual;
    if(*tabela)
    {
        atual = *tabela;
        while(atual->prox)
        {
            atual = atual->prox;
        }
        atual->prox = criaNoTabela(linha);
    }
    else
    {
        *tabela = criaNoTabela(linha);
    }
}
// DECODIFICACAO*****************************************************************
void decodificarTexto(Arvore *raiz, char *fraseCodificada, Tabela *tabela)
{
    Arvore *atual = raiz;
    Tabela *atualTab;
    char texto[4000];
    int simbolo;
    
    memset(texto, '\0', sizeof(texto));
    for(int i=0; i<strlen(fraseCodificada); i++)
    {
        if(atual->dir==NULL && atual->esq==NULL)
        {
            simbolo = atual->simbolo;
            atualTab = tabela;
            while(atualTab && simbolo!=atualTab->info.simbolo)
                atualTab = atualTab->prox;
            if(atualTab) //achei o simbolo
            {
                strcat(texto,atualTab->info.palavra);
            }
            atual = raiz;
        }

        if(fraseCodificada[i]=='1') //ando para a direita
        {
            atual = atual->dir;
        }
        else //ando para a esquerda
        {
            atual = atual->esq;
        }
    }
    if(atual->dir==NULL && atual->esq==NULL)
    {
        simbolo = atual->simbolo;
        atualTab = tabela;
        while(atualTab && simbolo!=atualTab->info.simbolo)
            atualTab = atualTab->prox;
        if(atualTab) //achei o simbolo
        {
            strcat(texto,atualTab->info.palavra);
        }
        atual = raiz;
    }

    // EXIBICAO DO TEXTO RECUPERADO DA DECODIFICACAO
    printf("\n\n%s\n",texto);
}

// INT MAIN**********************************************************************
int main()
{
    FILE *ptrBits = fopen("numeroBits.dat","rb");
    FILE *ptrTabela = fopen("tabela.dat","rb");
    Arvore *raiz = NULL;
    Tabela *tabela = NULL, *atual;
    LinhaTabela linha;
    char fraseCodificada[500];
    int restantes;

    memset(fraseCodificada,'\0',sizeof(fraseCodificada)); // INICIO A FRASE
    lerArqFraseCodificada(fraseCodificada); //LEIO A FRASE DO ARQUIVO BINARIO
    printf("Frase codificada:\n%s\n", fraseCodificada); // EXIBO A FRASE LIDA DO ARQ. BI.

    // CONSTRUCAO DA ARVORE A PARTIR DA TABELA DO ARQ. BI.
    construirArvoreBB(&raiz);

    // EXIBICAO DA ARVORE GERADA
    printf("\n\nArvore gerada:\n");
    exibeArvore(raiz);

    // LEITURA DOS BITS "A MAIS" NA MINHA STRING CODIFICADA
    fread(&restantes,sizeof(int),1,ptrBits);
    
    // EXIBICAO DOS BITS "A MAIS" PARA SABER SE MINHA LEITURA ESTA CORRETA
    // printf("\n%d\n",restantes);
    // int TL = strlen(fraseCodificada);

    // CORRECAO DA MINHA STRING A USANDO O NUMERO DE BITS COMO O MEU NUMERO DE INTERACOES
    for(int i=0; i<restantes; i++)
    {
        fraseCodificada[strlen(fraseCodificada)-1] = '\0';
    }

    // EXIBICAO DA FRASE CODIFICADA CORRIGIDA
    // printf("\n\nFrase codificada novamente\n");
    // printf("%s\n",fraseCodificada);
    fcloseall();

    // COLOCAR A TABELA DO ARQ. BI. EM LISTA ENCADEADA PARA MANIPULACAO MAIS RAPIDA
    fread(&linha,sizeof(LinhaTabela),1,ptrTabela);
    while(!feof(ptrTabela))
    {
        //printf("%s\n",linha.palavra); // EXIBICAO DAS PALAVRAS DO ARQ. BI.
        inserirTabela(&tabela, linha);
        fread(&linha,sizeof(LinhaTabela),1,ptrTabela);
    }

    // EXIBICAO DA LISTA AGORA EM MEMORIA
    // atual = tabela;
    // while(atual)
    // {
    //     printf("%s - ",atual->info.palavra);
    //     printf("%s - ",atual->info.codigo);
    //     printf("%d\n",atual->info.simbolo);
    //     atual = atual->prox;
    // }
    fcloseall();

    // DECODIFICACAO DA STRING DO ARQUIVO BINARIO
    decodificarTexto(raiz,fraseCodificada,tabela);
    // A EXIBICAO DO TEXTO RECUPERADO DA STRING CODIFICADA OCORRE DENTRO DA FUNCAO

    return 0;
}