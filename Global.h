#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <ctype.h>

//struct de arestas do grafo, ou seja, das listas encadeadas
typedef struct a{
	char nomeEst[64];
	int dist;
	char nomeLinha[64][64];
	struct a *prox;  //ponteiro prox
} aresta;

//struct de vertices do grafo 
typedef struct v{
	char nomeEst[64];
	aresta *first;
} vertice;

//struct auxiliar do algoritmo Dijkstra
typedef struct c{
	char ANT[64];
	int cor;        //0=branco, 1=preta
	int D;
	//int valorAnt;
} vert_visita;

//struct auxiliar do algoritmo Prim
typedef struct b{
	bool conn;
	int pai;
	int dist;
} aux_prim;

typedef aresta *aresta_ptr;

#define TAM_VERTICE 1000

#endif