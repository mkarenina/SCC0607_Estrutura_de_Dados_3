#ifndef PRINTS_H
#define PRINTS_H

#include "Global.h"

void printGrafo(vertice *vert[], int at);
void printBusca(char *destino, char *printErro, int destPos, char *origem, vertice *vet[], int conectado, vert_visita *visitado[]);

#endif