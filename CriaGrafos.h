

#ifndef CRIAGRAFOS_H
#define CRIAGRAFOS_H

#include "Global.h"

int ordenacaoVertice(char estacao[], vertice *vert[]);
void ordenacaoLinhas(aresta_ptr atual, char *linhaInsercao);
void ordenacaoAresta(aresta_ptr insercao, vertice *vert[], int posAt, char *nomeLinha);
int verifica_removido(FILE *arq);
int aresta_integracao(FILE *busca, vertice *ver[], int codEstIntegra, int vertice_at);
aresta_ptr percorreAresta (char nomeEst_original[], vertice *origem[], int posAtual);
int percorreVertice (char origem[], vertice *vert[]);
void criaAresta(int codProxEst, vertice *vert[], int i, int distancia, char nomeLinha[], FILE *busca, char *nomeEst);
int ConexaoVertices(vertice *vertOrigem, vertice *VertDest);
void GrafoNaoDirecionado(vertice *vert[], int qtdeVert);
int criaGrafo(FILE *arq, FILE *busca, vertice *vert[], int TipoGrafo);

#endif