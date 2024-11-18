#ifndef F195E25B_8935_401C_8555_78D07D73B881
#define F195E25B_8935_401C_8555_78D07D73B881

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct pilhaString {
    char info[40];
    struct pilhaString *prox;
} PilhaString;

void initStringPilha(PilhaString **P) {
    *P = NULL;
}

char isEmptyStringPilha(PilhaString *P) {
    return P == NULL;
}

void pushStringPilha(PilhaString **P, char *info) {
    PilhaString *nova = (PilhaString *) malloc(sizeof(PilhaString));
    strcpy(nova->info, info);
    nova->prox = *P;
    *P = nova;
}

void printarStringPilha(PilhaString *P) {
    while (!isEmptyStringPilha(P)) {
        printf("%s\n", P->info);
        P = P->prox;
    }
}

void topStringPilha(PilhaString *P, char *info) {
    if (!isEmptyStringPilha(P)) {
        strcpy(info, P->info);
    }
}

void popStringPilha(PilhaString **P, char *info) {
    if (!isEmptyStringPilha(*P)) {
        PilhaString *aux = *P;
        strcpy(info, aux->info);
        *P = (*P)->prox;
        free(aux);
    }
}

int tamanhoStringPilha(PilhaString *P) {
    int cont = 0;
    while (P != NULL) {
        cont++;
        P = P->prox;
    }
    return cont;
}

#endif /* F195E25B_8935_401C_8555_78D07D73B881 */
