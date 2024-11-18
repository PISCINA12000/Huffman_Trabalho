#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct arvoreHuf
{
    int soma, freq, simbolo; // quando a soma for zero então sei que é uma folha
    struct arvoreHuf *esq;
    struct arvoreHuf *dir;
};
typedef struct arvoreHuf ArvoreHuf;

struct pilha{
    ArvoreHuf *arvore;
    struct pilha *prox;
};
typedef struct pilha Pilha;

// Inicializa a pilha (define o topo como NULL).
void init(Pilha** P) {
    *P = NULL;
}

// Verifica se a pilha está vazia.
char isEmpty(Pilha* P) {
    return P == NULL;
}

// Empilha um ponteiro para uma estrutura Tree na pilha.
void push(Pilha** P, ArvoreHuf *x) {
    Pilha *nova = (Pilha*) malloc(sizeof(Pilha));
    nova->arvore = x;
    nova->prox = *P;
    *P = nova;
}

// Retorna o ponteiro para a estrutura Tree no topo da pilha.
void top(Pilha* P, ArvoreHuf** arvoreHuf) {
    if (!isEmpty(P)) {
        *arvoreHuf =  P->arvore;
    }
    *arvoreHuf = NULL;
}

// Desempilha um ponteiro para uma estrutura Tree e o retorna via referência.
void pop(Pilha** P, ArvoreHuf** arvoreHuf) {
    Pilha* aux;
    ArvoreHuf *auxTree;
    if (!isEmpty(*P)) {
        auxTree = (*P)->arvore;
        aux = *P;
        *P = (*P)->prox;
        free(aux);
    } else {
        auxTree = NULL;
    }
    *arvoreHuf = auxTree;
}

// Retorna o tamanho da pilha.
int tamanho(Pilha* P) {
    int cont = 0;
    while (P != NULL) {
        cont++;
        P = P->prox;
    }
    return cont;
}
