#include "CriaGrafos.h"
#include "Buscas.h"


//Aloca memoria para o vertice atual e add na posicao correta do vetor
int ordenacaoVertice(char estacao[], vertice *vert[]){
	int i = 0;
	int at;
	vertice *temp, *atual;
	
	while(vert[i]!=NULL && strcmp(estacao, vert[i]->nomeEst)>0){
		i++;
	}

	if(vert[i]== NULL){      //Tem que ser inserido na última posição do vetor
		vert[i]=(vertice*)calloc(1,sizeof(vertice));
		vert[i]->first = NULL;
		strcpy(vert[i]->nomeEst, estacao);
		return i;
	}

	if(strcmp(estacao, vert[i]->nomeEst)==0){  //vertice ja existe
		return i;
	}


	//Inserir vertice no meio da lista
	atual=(vertice*)calloc(1,sizeof(vertice));
	atual->first = NULL;
	strcpy(atual->nomeEst, estacao);
	
	at = i;
	

	while(vert[i]!= NULL){
		temp = vert[i];
		vert[i] = atual;
		atual = temp;
		i++;
	}

	vert[i]=atual;
	return at;      //Indica que vertice ja foi inserido no meio da lista
}

//Insere linhaInsercao na posicao ja ordenada da aresta
void ordenacaoLinhas(aresta_ptr atual, char *linhaInsercao){
	int i=0;
	char temp[64];

	while(strcmp(atual->nomeLinha[i], "\0")!=0 && strcmp(linhaInsercao, atual->nomeLinha[i])>0){
		//Enquanto nao chegar no final das linhas ou não encontrar o lugar para inserir
		i++;
	}

	if(strcmp(atual->nomeLinha[i], "\0")==0){
		//Se encontrou lugar no final
		strcpy(atual->nomeLinha[i], linhaInsercao);
		return;
	}

	//Se encontrou lugar no meio
	while(strcmp(atual->nomeLinha[i], "\0")!=0){
		strcpy(temp, atual->nomeLinha[i]);
		strcpy(atual->nomeLinha[i], linhaInsercao);
		strcpy(linhaInsercao, temp);
		i++;
	}

	strcpy(atual->nomeLinha[i], linhaInsercao);

	return;
}

//Encontra posicao para inserir aresta atual QUE JA FOI ALOCADA e insere
void ordenacaoAresta(aresta_ptr insercao, vertice *vert[], int posAt, char *nomeLinha){
	aresta_ptr atual = vert[posAt]->first;
	aresta_ptr ant = NULL;
	

	if(vert[posAt]->first == NULL){
		vert[posAt]->first = insercao;
		return;
	}

	while(atual!=NULL && strcmp(insercao->nomeEst, atual->nomeEst)>0){    //Enquanto não encontra lugar p add
		ant=atual;
		atual=atual->prox;
	}

	if(atual==NULL){
		strcpy(insercao->nomeLinha[0], nomeLinha);
		ant->prox = insercao;
		return;
	}

	if(strcmp(insercao->nomeEst, atual->nomeEst)==0){   //Se já existe aresta, só add nome da linha ordenado
		ordenacaoLinhas(atual, nomeLinha);
		return;
	}

	if(ant==NULL){  //Primeiro da lista de arestas
		insercao->prox = atual;
		vert[posAt]->first = insercao;
		return;
	}

	//Se tem que add aresta atual no meio
	strcpy(insercao->nomeLinha[0], nomeLinha);
	insercao->prox = ant->prox;
	ant->prox = insercao;
	return;
}

//Verifica se o dado atual foi removido
int verifica_removido(FILE *arq){
	char removido;
	removido = getc(arq);
	if(removido == '1'){
		return 1;
	}

	return 0;
}

//Verifica estacao de integracao e add no grafo se ela nao for o vertice atual
int aresta_integracao(FILE *busca, vertice *ver[], int codEstIntegra, int vertice_at){
	char nomeEstIntegra[64];
	char integracao[20] = "Integracao";
	aresta_ptr atual = NULL;
	int erro_busca;

	erro_busca = buscar(codEstIntegra, busca, nomeEstIntegra);
	if(erro_busca==1){
		return 1;
	}

	if(strcmp(nomeEstIntegra, ver[vertice_at]->nomeEst)==0){    //Se integracao = estacao atual
		return 0;
	}

	//Se nao for igual, cria aresta e aloca memória
	atual = (aresta_ptr)calloc(1,sizeof(aresta));
	atual->dist=0;
	strcpy(atual->nomeEst, nomeEstIntegra);
	strcpy(atual->nomeLinha[0], integracao);
	atual->prox=NULL;

	ordenacaoAresta(atual, ver, vertice_at, integracao);

	return 0;
}

//Funcao que percorre arestas, retorna 0 se nao existe aresta com msm nomeEst e 1 se existe aresta c msm nomeEst
aresta_ptr percorreAresta (char nomeEst_original[], vertice *origem[], int posAtual){
	aresta_ptr atual = origem[posAtual]->first;

	while((atual != NULL) && (strcmp(nomeEst_original, atual->nomeEst)) != 0){
		atual = atual->prox;
	}

	return atual;
}

int percorreVertice (char origem[], vertice *vert[]){
	int i=0;
	while(vert[i]!=NULL && strcmp(vert[i]->nomeEst, origem)!=0){
		i++;
	}

	if(vert[i]==NULL){
		return -1;      //Se nao encontrou vertice buscado, retorna erro
	}

	return i;
}

//Cria aresta (sabendo que ela nao existe previamente)
void criaAresta(int codProxEst, vertice *vert[], int i, int distancia, char nomeLinha[], FILE *busca, char *nomeEst){
	char nomeProxEst[64];
	int retorno_erro;
	aresta_ptr insercao, arestaAnterior;

	arestaAnterior=vert[i]->first;
	
	if(busca){	//Percorrer arquivo para encontrar
		retorno_erro = buscar(codProxEst, busca, nomeProxEst);
		if (retorno_erro==1){
			return;       //Nao encontrou nomeEst com esse codigo, pular
		}
	}else{
		strcpy(nomeProxEst, nomeEst);
	}
	
	
	//Encontrou nome da estacao que sera add na aresta
	//Aloca memória e insere os dados
	insercao = (aresta_ptr)calloc(1,sizeof(aresta));
	insercao->dist = distancia;
	strcpy(insercao->nomeEst, nomeProxEst);
	strcpy(insercao->nomeLinha[0], nomeLinha);
	insercao->prox = NULL;


	if(arestaAnterior!=NULL){   //Nao eh a primeira aresta do vertice
		//Ordena a aresta alocada, arrumando os ponteiros de insercao
		ordenacaoAresta(insercao, vert, i, nomeLinha);
	}else{
		vert[i]->first = insercao;
	}

	return;
}

int ConexaoVertices(vertice *vertOrigem, vertice *VertDest){
	aresta_ptr atual = vertOrigem->first;

	while(atual!=NULL){
		if(strcmp(atual->nomeEst, VertDest->nomeEst)==0){
			return 1;
		}
		atual=atual->prox;
	}
	return 0;
}

//Funcao para funcionalidade 10
//Cria grafo nao direcionado
void GrafoNaoDirecionado(vertice *vert[], int qtdeVert){
	int i, j, existeConec, posVert;
	vertice *vertAt;
	aresta_ptr atual, existeAresta;

	for(i=0; i<qtdeVert; i++){
		vertAt=vert[i];
		atual=vertAt->first;
		while(atual!=NULL){
			posVert=percorreVertice(atual->nomeEst, vert);
			existeConec=ConexaoVertices(vert[posVert], vertAt);
			if(!existeConec){
                criaAresta(0, vert, posVert, atual->dist, atual->nomeLinha[0], NULL, vertAt->nomeEst);
                existeAresta = percorreAresta(vertAt->nomeEst, vert, posVert);
                for(j=1; atual->nomeLinha[j][0]!='\0'; j++){
                	ordenacaoLinhas(existeAresta, atual->nomeLinha[j]);
                }
			}
			atual=atual->prox;
		}
	}
	return;
}

//Cria o grafo, se tipografo -> 1 grafo nao ordenado
int criaGrafo(FILE *arq, FILE *busca, vertice *vert[], int TipoGrafo){
	int tamRegistro, distancia, i, tamNomeLinha, codProxEst, codEstInt, retorno_erro, tamNomeEst;
	char removido;
	char nomeEst[64], nomeLinha[64], nomeProxEst[64];
	aresta_ptr insercao = NULL;
	aresta_ptr existeAresta;

	pula_cabecalho(arq);
	
	while((removido = getc(arq)) != EOF){
		fread(&tamRegistro, 1, sizeof(int), arq);
		if(removido==1){    //Se registro foi removido, pular
			fseek(arq, tamRegistro, SEEK_CUR);
		}else{
			fseek(arq, 16, SEEK_CUR);   //Chega na distancia até prox estacao
			fread(&codProxEst, 1, sizeof(int), arq);
			fread(&distancia, 1, sizeof(int), arq);
			fseek(arq, 4, SEEK_CUR);    //Pula codLinhaIntegra
			fread(&codEstInt, 1, sizeof(int), arq);
			tamNomeEst=read_field(arq, '|', nomeEst);
			nomeEst[tamNomeEst-1]='\0';
			tamNomeLinha = read_field(arq, '|', nomeLinha);
			nomeLinha[tamNomeLinha-1]='\0';

			i=0;
			while(vert[i]!=NULL && strcmp(vert[i]->nomeEst, nomeEst)!=0){
				i++;
			}

			if(vert[i]==NULL){      //Insere novo vertice
				i = ordenacaoVertice(nomeEst, vert);

				if(codProxEst!=-1){//Se existir codProxEst
					//Criacao da aresta!
					//Encontrou nome da estacao que sera add na aresta
					
					criaAresta(codProxEst, vert, i, distancia, nomeLinha, busca, NULL);
				}else{
						vert[i]->first = NULL;
				}

				if(codEstInt!=-1){  //Existe estacao de integracao
					retorno_erro=aresta_integracao(busca, vert, codEstInt, i);
				
				}

			}else{  //Vértice já existe, só inserir a aresta atual
				if(codProxEst!=-1){         //Se existir nome da linha
					//Funcao que percorre arestas, retorna 0 se nao existe aresta com msm nomeEst e 1 se existe aresta c msm nomeEst
					existeAresta = percorreAresta(nomeProxEst, vert, i);
					if(existeAresta!=NULL){    //Aresta ja existe, so add nova linha
						ordenacaoLinhas(existeAresta, nomeLinha);
					}else{                  //Aresta n existe, precisa ser criada e add corretamente
						//Criacao da aresta!
						criaAresta(codProxEst, vert, i, distancia, nomeLinha, busca, NULL);
						
					}
				}
				if(codEstInt!=-1){  //Existe estacao de integracao
					retorno_erro=aresta_integracao(busca, vert, codEstInt, i);
					
				}
			}
		}
	} 
	return 0;
}