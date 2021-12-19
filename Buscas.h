#ifndef BUSCAS_H
#define BUSCAS_H

#include "Global.h"

void pula_cabecalho(FILE *arq);
int read_field(FILE *arq, char delim, char *buffer);
int buscar(int codEst, FILE *arq, char *nomeEst);
int recBuscaProfundidade(vertice *vert[], char *origem, vert_visita *visitado[], int posAt);
void buscaProfundidade(vertice *vert[], char *estacaoOrigem, int totalV);

  
#endif

