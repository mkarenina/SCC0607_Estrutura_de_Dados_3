
#include "Prints.h"
#include "CriaGrafos.h"

//Printa 1 vertice por vez
void printGrafo(vertice *vert[], int at){
	aresta_ptr atual;
	int i;
	vertice *ver_at = vert[at];

	printf("%s", vert[at]->nomeEst);

	if(vert[at]->first!=NULL){  //Se vértice possui aresta
		printf(" ");
	}
	
	atual = vert[at]->first;
	while(atual!=NULL){
		printf("%s %d", atual->nomeEst, atual->dist);
		for(i=0; strcmp(atual->nomeLinha[i], "\0")!=0; i++){
			printf(" %s", atual->nomeLinha[i]);
		}
		atual=atual->prox;
		if(atual!=NULL){
			printf(" ");
		}
	}
	return;
}

//Funcao que printa as buscas do Dijkstra e de grafo ciclico
void printBusca(char *destino, char *printErro, int destPos, char *origem, vertice *vet[], int conectado, vert_visita *visitado[]){
	int cont=0;
	int Dtot=0;
	int pos = destPos;
	int i, j;

	while(pos!=-1 && strcmp(visitado[pos]->ANT, "\0")!=0){
		if(strcmp(visitado[pos]->ANT, origem)==0){
			break;
		}
        cont ++;
		Dtot+=visitado[pos]->D;
		pos=percorreVertice(visitado[pos]->ANT, vet);
		
	}

	cont ++;
	Dtot+=visitado[pos]->D;
	pos=percorreVertice(visitado[pos]->ANT, vet);

	if(conectado==0 || conectado==-1){	//Nao encontrou estacao
		printf("%s\n", printErro);
		return;
	}

	printf("Numero de estacoes que serao percorridas: %d\n", cont );
	
	if(strcmp(destino, origem)==0){
		printf("Distancia que sera percorrida: %d\n", Dtot);
	}else{
		printf("Distancia que sera percorrida: %d\n", visitado[destPos]->D);
	}
	

	for(i = cont-1; i >= 0; i--){
		pos = destPos;
		for(j = 0; j < i; j++){
			pos=percorreVertice(visitado[pos]->ANT, vet);
		}
		printf("%s, ", visitado[pos]->ANT);
	}
	
	printf("%s\n", destino);
}