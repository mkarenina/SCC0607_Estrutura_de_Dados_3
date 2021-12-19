#include "Prim.h"
#include "CriaGrafos.h"


//Função que realiza o algoritmo Prim
//Recebe um grafo nao-direcionado conexo com as distancias como pesos e calcula a arvore geradora minima
//Funcao recursiva com busca em profundidade que printa a resposta para o Prim
int recBuscaProfundidade_prim(vertice *vert[], aresta_ptr* arestas, char *origem, int posAt){
	int posFilho, respostaRec;
	aresta_ptr curr_a = arestas[posAt];
	aresta_ptr avanco = NULL;

	
	while(curr_a!=NULL){	//Percorre as arestas para pegar a de menor caminho
		printf("%s, ", vert[posAt]->nomeEst);	//Printa nome da estacao atual
		printf("%s, ", curr_a->nomeEst);
		printf("%d\n", curr_a->dist);

		posFilho = percorreVertice(curr_a->nomeEst, vert);

		respostaRec = recBuscaProfundidade_prim(vert, arestas, origem, posFilho);
		curr_a=curr_a->prox;
	}
	return -1;
}

//Percorre grafo em profundidade para encontrar ciclo
void buscaProfundidade_prim(vertice *vert[], aresta_ptr* arestas, char *estacaoOrigem, int totalV){
	int posAt, posTeste, tmp, i, resposta;
	int running=1;

	aresta_ptr curr_a, avanco;
	vertice *curr_v;
	
	posAt = percorreVertice(estacaoOrigem, vert);
	if(posAt==-1){
		return;
	}

	curr_a = arestas[posAt];
	avanco = curr_a;

	resposta = recBuscaProfundidade_prim(vert, arestas, estacaoOrigem, posAt);

	return;
}

aresta_ptr* Prim (char origem[], vertice *verts[], int total_vertices){
	
	//variaveis 
	aresta_ptr *res =(aresta_ptr *)malloc(sizeof(aresta_ptr)*total_vertices);
 	aux_prim vet[total_vertices];
	int min, pos, tmpPos;
	aresta_ptr aresta_vet;
	int i,j;
	
	//inicialização do algoritmo
	for(i = 0; i < total_vertices; i ++){
		res[i] = NULL;
		vet[i].conn = false;
		vet[i].pai  = -1;
		vet[i].dist = INT_MAX/2;
	}

	//encontra posicao de origem
	pos = percorreVertice(origem, verts);

	//verificação de falha
	if(pos==-1){
		printf("Falha percorre Vertice.");
	}

	//inicializa verice origem
	vet[pos].conn = true;
	vet[pos].dist = 0;

	//percorre todos os vertices 
	aresta_vet = verts[pos]->first;
	while(aresta_vet!= NULL){
		tmpPos = percorreVertice(aresta_vet->nomeEst, verts);
		vet[tmpPos].dist = aresta_vet->dist;
		vet[tmpPos].pai = pos;
		aresta_vet = aresta_vet->prox;
	}
	
	//compara as distancias e escolhe sempre a menor aresta possível para fazer parte da arvore geradora minima
	while(true){
		min = INT_MAX/4;
		pos = -1;
		for(i = 0; i < total_vertices; i++){
			if(vet[i].conn == false && vet[i].dist < min){
				min = vet[i].dist;
				pos = i;
			}
		}
		if(pos == -1){
			break;
		}

		aresta_vet = verts[pos]->first;
		vet[pos].conn = true;
	
		while(aresta_vet!= NULL){
			tmpPos = percorreVertice(aresta_vet->nomeEst, verts);
			if(vet[tmpPos].conn == false && aresta_vet->dist < vet[tmpPos].dist){
				vet[tmpPos].dist = aresta_vet->dist;
				vet[tmpPos].pai = pos;
			}
			aresta_vet = aresta_vet->prox;
		}
	}

	//verificacao de erro no calculo da arvore
	for(i = 0; i < total_vertices; i++){
		res[i] = NULL;
		if(vet[i].conn == false){
			printf("Falha na execução da funcionalidade.\n");
			return NULL;
		}
	}

	//geracao da arvore com base no que foi calculado
	for(i = 0; i < total_vertices; i++){
		pos = vet[i].pai;
		
		if(pos == -1){
		}else if(res[pos] == NULL){
			res[pos] = (aresta_ptr)malloc(sizeof(aresta));
			strcpy(res[pos]->nomeEst, verts[i]->nomeEst);
			
			res[pos]->dist = vet[i].dist;
			res[pos]->prox = NULL;

			
		}else{
	
			aresta_ptr tmpAr = res[pos];
			while(tmpAr->prox != NULL){
				tmpAr = tmpAr->prox;
			}

			tmpAr->prox = (aresta_ptr)calloc(sizeof(aresta),1);
			tmpAr = tmpAr->prox;

			strcpy(tmpAr->nomeEst, verts[i]->nomeEst);
			tmpAr->dist = vet[i].dist;
			tmpAr->prox = NULL;
		}
	}
	
	return res;
}