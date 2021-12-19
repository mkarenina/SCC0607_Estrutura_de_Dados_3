#include "Buscas.h"
#include "CriaGrafos.h"
#include "Prints.h"

//Funcao que pula o cabecalho
void pula_cabecalho(FILE *arq){
	fseek(arq, 17, SEEK_SET);
	return;
}

//Essa funcao le o arquivo e coloca no buffer todos os caracteres ate encontrar o delim escolhido, no caso "|"
int read_field(FILE *arq, char delim, char *buffer){
	char c=' ';
	int i;

	for(i=0;c!=delim && c!='\n';i++){
		fread(&c,1,1,arq);
		buffer[i]=c;
	}

	buffer[i]='\0';

	return i;
}

//Busca no arquivo o nome da estacao que possui o código passado como parametro
int buscar(int codEst, FILE *arq, char *nomeEst){
	char c;
	int codAt, tamReg, tamEst;

	pula_cabecalho(arq);

	while((c=getc(arq)!=EOF)){
		fread(&tamReg, 1, sizeof(int), arq); //Le tamanho do registro atual p pular caso nao seja o registro certo
		fseek(arq, 8, SEEK_CUR);            //Chega em codEst
		fread(&codAt, 1, sizeof(int), arq);  //Le codEst para comparar com o cod da estacao que estamos buscando
		if(codAt==codEst){                  //Achou o dado com o nome da estacao!
			fseek(arq, 20, SEEK_CUR);
			tamEst=read_field(arq, '|', nomeEst);
			nomeEst[tamEst-1]='\0';
			return 0;
		}
		fseek(arq, (tamReg-12), SEEK_CUR);
	}
	
	return 1;   //Erro, nao encontrou nome da estacao
}

//Função recursiva da busca de profundidade para encontrar o ciclo nas estações
int recBuscaProfundidade(vertice *vert[], char *origem, vert_visita *visitado[], int posAt){
	int posTeste, respostaRec;
	aresta_ptr curr_a = vert[posAt]->first;
	aresta_ptr avanco = NULL;
	
	while(curr_a!=NULL){	//Percorre as arestas para pegar a de menor caminho
		if(strcmp(curr_a->nomeEst, origem)==0){	//Encontrou ciclo!
			posTeste = percorreVertice(curr_a->nomeEst, vert);
			if(posTeste==-1){	//Nao encontrou o vertice que estamos buscando
				return -1;
			}
			strcpy(visitado[posTeste]->ANT, vert[posAt]->nomeEst);	//Atualizando struct visitado
			visitado[posTeste]->cor = 1;
			visitado[posTeste]->D = curr_a->dist;
			return posTeste;
		}
		posTeste = percorreVertice(curr_a->nomeEst, vert);
		if(posTeste==-1){	//Nao encontrou o vertice que estamos buscando
				return -1;
		}
		if(visitado[posTeste]->cor==0){		//Se o vertice ainda nao foi analisado, vai para ele
			avanco=curr_a;
			strcpy(visitado[posTeste]->ANT, vert[posAt]->nomeEst);	//Atualizando struct visitado
			visitado[posTeste]->cor = 1;
			visitado[posTeste]->D = avanco->dist;

			respostaRec = recBuscaProfundidade(vert, origem, visitado, posTeste);
			if(respostaRec!=-1){		//Encontrou ciclo
				return respostaRec;
			}
		}
		curr_a=curr_a->prox;
	}

	return -1;
}

//Percorre grafo em profundidade para encontrar ciclo
void buscaProfundidade(vertice *vert[], char *estacaoOrigem, int totalV){
	int posAt, posTeste, tmp, i, resposta;
	int running=1;

	aresta_ptr curr_a, avanco;
	vertice *curr_v;
	vert_visita *visitado[TAM_VERTICE];

	for(i=0; i<totalV; i++){
		visitado[i]=(vert_visita *)calloc(1,sizeof(vert_visita));
		visitado[i]->cor=0;		//Branco
	}
	
	posAt = percorreVertice(estacaoOrigem, vert);
	if(posAt==-1){
		return;
	}

	visitado[posAt]->D=0;
	visitado[posAt]->cor=1;

	curr_a=vert[posAt]->first;
	avanco = curr_a;

	resposta = recBuscaProfundidade(vert, estacaoOrigem, visitado, posAt);

	printBusca(estacaoOrigem, "Nao existe um ciclo a partir da estacao de origem.", resposta, estacaoOrigem, vert, resposta, visitado);
	
	return;

}

