#include "Dijkstra.h"
#include "Prints.h"
#include "CriaGrafos.h"

//Funcao que realiza o algoritmo Dijkstra que percorre o vetor até achar menor caminho entre origem e destino
void Dijkstra(char origem[], char destino[], vertice *vet[], int total_vertices){
	int pos;            //Posicao do vertice atual que o Dijkstra está analisando
	int running,tmp, a, curr_int;
	int cont = 0;
	int i;
	int conectado = 0;
	int acabou=0;	//Recebe 1 se as estacoes estao conectadas
	int destPos = percorreVertice(destino, vet);
	
	aresta_ptr curr_a;
	vertice *curr_v;
	vert_visita *visitado[total_vertices];

	for(i=0; i<total_vertices; i++){
		visitado[i]=(vert_visita *)calloc(1,sizeof(vert_visita));
		visitado[i]->cor=0;		//Branco
		memset(visitado[i]->ANT ,'\0',sizeof(char));
		visitado[i]->D=10000000;
	}

	pos=percorreVertice(origem, vet);

	if(pos==-1){
		printf("Falha na execução da funcionalidade.\n");
	}

	curr_v = vet[pos];
	visitado[pos]->D = 0;     //pos é a posição do vertice inicial

	running = 1;
	curr_a=curr_v->first;

	while(running){	//Relaxação

		while(curr_a!=NULL){ //Atualiza todas as arestas adjacentes com os menores valores possíveis
			tmp=percorreVertice(curr_a->nomeEst, vet); // tmp recebe a estacao que estamos analizando na aresta
			if(visitado[tmp]->cor==0){  //Vertice está aberto
				if(visitado[tmp]->D>(curr_a->dist+visitado[pos]->D)){
					strcpy(visitado[tmp]->ANT, vet[pos]->nomeEst);
					visitado[tmp]->D = (visitado[pos]->D+curr_a->dist);
				}
				if(strcmp(curr_a->nomeEst, destino)==0){
					conectado=1;
				}
			}
			curr_a=curr_a->prox;
		}
	
		curr_int = 0;
		for(tmp=0; tmp<total_vertices; tmp++){  //Pula para o proximo vertice
			if(visitado[tmp]->cor == 0 && strcmp(visitado[tmp]->ANT, "\0")!=0){   //Se vertice adjacente está aberto e ja foi analisado no while anterior
				if(visitado[pos]->cor == 1 || visitado[curr_int]->D >= visitado[tmp]->D){
					if(visitado[pos]->cor == 0 && visitado[curr_int]->D==visitado[tmp]->D){ 
						if(strcmp(vet[pos]->nomeEst, vet[tmp]->nomeEst)>0){     //2 antes que 1                            
							curr_int = tmp;
							pos = tmp;
						}
					} else {
						curr_int = tmp;
						pos = tmp;
					}
				}
			}
		}
		
		if(visitado[pos]->cor){
			running=0;
		}

		visitado[pos]->cor = 1;
		
        curr_v= vet[pos];
		curr_a=curr_v->first;
	}

	printBusca(destino, "Não existe caminho entre as estações solicitadas.", destPos, origem, vet, conectado, visitado);

    //free visitado
    for(i=0; i<total_vertices; i++){
		free(visitado[i]);
	}
    
	return;
}