#ifndef PRIM_H
#define PRIM_H

#include "Global.h"

int recBuscaProfundidade_prim(vertice *vert[], aresta_ptr* arestas, char *origem, int posAt);
void buscaProfundidade_prim(vertice *vert[], aresta_ptr* arestas, char *estacaoOrigem, int totalV);
aresta_ptr* Prim (char origem[], vertice *verts[], int total_vertices);

#endif