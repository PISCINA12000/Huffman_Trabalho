#include <stdlib.h>
#include <stdio.h>
#include "TAD_PilhaEnd.h"
#include "Structs.h"

//INICIALIZACAO
void InitTree(Tree **tree){
    *tree = NULL;
}

//VERIFICAR SE ESTA VAZIA
char TreeIsEmpty(Tree *tree){
    return tree == NULL;
}

//FAZ UMA ALOCACAO DE MEMORIA E ATRIBUI ESSE ENDERECO DE MEMORIA AO RETORONO DA FUNCAO
Tree* CriaNoTree(int info){
    Tree *no = (Tree*)malloc(sizeof(Tree));
    no->dir = no->esq = NULL;
    no->info = info;
    return no;
}

//INSERE NA ARVORE ALGUM ELEMENTO, DE FORMA ITERATIVA
void InserirTreeIterativo(Tree **tree, int info){
    Tree *aux = *tree, *no = CriaNoTree(info);
    if(aux) //tenho alguma coisa na minha arvore, posso comecar a achar a posicao correta
    {
        while(aux){
            if(info > aux->info) //vou andar para a direita
            {
                if(aux->dir) //existe algo na ramificacao a direita, tenho que continuar andando
                {
                    aux = aux->dir;
                }
                else //adiciono na arvore
                {
                    aux->dir = no;
                    aux = no->dir; //isso fara com que a repeticao acabe
                }
            }
            else //vou andar para a esquerda
            {
                if(aux->esq) //existe algo na ramificacao a esquerda, tenho que continuar andando
                {
                    aux = aux->esq;
                }
                else //adiciono na arvore, pois nao ha nenhum elemento onde ele deveria estar
                {
                    aux->esq = no;
                    aux = no->esq;
                }
            }
        }
    }
    else //a arvore esta vazia
    {

        //a raiz da arvore recebe o proprio novo no
        *tree = no;
    }
}

//INSERE NA ARVORE ALGUM ELEMENTO, DE FORMA RECURSIVA
void InserirTreeRecursivo(Tree **tree, int info){

    if(*tree)
    {
        if(info > (*tree)->info) //olhar elementos a direita
        {
            if((*tree)->dir) //possui elemento(s) na direita dessa raiz
            {
                InserirTreeRecursivo(&(*tree)->dir, info);
            }
            else //adiciono onde deve estar
            {
                (*tree)->dir = CriaNoTree(info);
            }
        }
        else //olhar elementos a esquerda
        {
            if((*tree)->esq) //possui elemento(s) na esquerda dessa raiz
            {
                InserirTreeRecursivo(&(*tree)->esq, info);
            }
            else //adiciono onde deve estar
            {
                (*tree)->esq = CriaNoTree(info);
            }
        }
    }
    else //nenhum elemento na raiz
    {
        *tree = CriaNoTree(info);
    }
}

//A PARTIR DE UM NUMERO, RETORNA O NIVEL DESSE NO EM RELACAO A ARVORE COMO UM TODO
int NivelNoTree(Tree *tree, int info) //nivel ou profundidade
{
    int nivel = 1;
    while(tree && tree->info != info)
    {
        if(info > tree->info) //elemento que procuro esta a direita
        {
            tree = tree->dir;
        }
        else
        {
            tree = tree->esq;
        }
        nivel++;
    }
    if(tree) //encontrei meu elemento
    {
        return nivel;
    }
    else
        return -1;
}

//RETORNO O PAI DE UM DETERMINADO NO PASSADO POR PARAMETRO
Tree* Pai(Tree *tree, int info)
{
	Tree *aux, *ant;
	if(tree) //nao esta nula
	{
		if(tree->info == info) //isso quer dizer que o elemento nao possui um pai
		{
			ant = NULL;
		}
		else //quer dizer que o elemento pode estar na arvore
		{
			//vou comecar a andar pela arvore para tentar achar o elemento
			aux = tree;
			while(aux && aux->info!=info)
			{
				ant = aux;
				if(info > aux->info) //andar para a direita
					aux = aux->dir;
				else //andar para a esquerda
					aux = aux->esq;
			}
			if(!aux) //nao achei o elemento, pontanto nao possui um pai para esse elemento
				ant = NULL;
		}
	}
	else //recebi uma arvore nula
	{
		printf("A arvore esta nula!!\n");
		ant = NULL;
	}
	
	return ant;
}

//Busca Iterativa
Tree* buscaABBI(Tree* tree, int info)
{
	while(tree && info != tree->info)
		if(info > tree->info)
			tree = tree->dir;
		else
			tree = tree->esq;
	return tree;
}

//Busca Recursiva 1
Tree* buscaABBR1(Tree* tree, int info)
{
	if(tree)
	{
		if(info == tree->info)
			return tree;
		else
			if(info > tree->info) //direita
				return buscaABBR1(tree->dir, info);
			else //esquerda
				return buscaABBR1(tree->esq, info);
	}
	else //nada na arvore
		return tree;
}

//Busca Recursiva 2
Tree* buscaABBR2(Tree* tree, int info)
{
	if(tree) //possui elementos na arvore
    {
        if(info != tree->info) //sao diferentes, ANDAR
            if(info > tree->info) //DIREITA
                return buscaABBR2(tree->dir,info);
            else //ESQUERDA
                return buscaABBR2(tree->esq,info);
    }
    return tree;
}

//Tornar a arvore Vazia
void Vazia_R(Tree **tree) //RECURSIVO
{
    if(*tree)
    {
        Vazia_R(&(*tree)->esq);
        Vazia_R(&(*tree)->dir);
        free(*tree);
    }
}
void Vazia_I(Tree **tree) //ITERATIVO
{
    Pilha *p; //será uma pilha de endereços
    init(&p);
    if(*tree)
    {
        while(*tree || !isEmpty(p))
        {
            while(*tree)
            {
                push(&p, *tree);
                *tree = (*tree)->esq;
            }
            if(!isEmpty(p))
            {
                *tree = top(p);
                *tree = (*tree)->dir;
                free(pop(&p));
            }
        }
    }
}

//TRAVESSIAS ITERATIVOS
void travessiaPreOrdem(Tree *tree) //PRÉ-ORDEM
{
    Pilha *p;
    init(&p);
    if(tree)
    {
        while(tree || !isEmpty(p))
        {
            while(tree)
            {
                printf("%d ",tree->info);
                push(&p, tree);
                tree = tree->esq;
            }
            if(!isEmpty(p))
            {
                tree = pop(&p);
                tree = tree->dir;
            }
        }
    }
}
void travessiaInOrdem(Tree *tree) //IN-ORDEM
{
    Pilha *p;
    init(&p);
    if(tree)
    {
        while(tree || !isEmpty(p))
        {
            while(tree)
            {
                push(&p, tree);
                tree = tree->esq;
            }
            if(!isEmpty(p))
            {
                tree = pop(&p);
                printf("%d ",tree->info);
                tree = tree->dir;
            }
        }
    }
}
void travessiaPosOrdem(Tree *tree) //PÓS-ORDEM
{
    Pilha *p, *p2;
    init(&p);init(&p2);
    if(tree)
    {
        while(tree || !isEmpty(p))
        {
            while(tree)
            {
                push(&p, tree);
                push(&p2,tree);
                tree = tree->dir;
            }
            if(!isEmpty(p))
            {
                tree = pop(&p);
                tree = tree->esq;
            }
        }
    }
    while(!isEmpty(p2))
    {
        tree = pop(&p2);
        printf("%d ",tree->info);
    }
}
//TRAVESSIAS RECURSIVO
void pre_ordem(Tree *tree)
{
    if(tree)
    {
        printf("%d ",tree->info);
        pre_ordem(tree->esq);
        pre_ordem(tree->dir);
    }
}
void in_ordem(Tree *tree)
{
    if(tree)
    {
        in_ordem(tree->esq);
        printf("%d ",tree->esq);
        in_ordem(tree->dir);
    }
}
void pos_ordem(Tree *tree)
{
    if(tree)
    {
        pos_ordem(tree->esq);
        pos_ordem(tree->dir);
        printf("%d ",tree->info);
    }
}