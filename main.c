/*Alunas:   Gabriela Vidal      ->  4277535
			Marianna Karenina   ->  10821144
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <ctype.h>

#define TAM_VERTICE 1000

//struct de arestas do grafo, ou seja, das listas encadeadas
typedef struct a{
	char nomeEst[64]; //codProxEst
	int dist;   //distancia
	char nomeLinha[64][64]; //nome das linhas
	struct a *prox;  //ponteiro prox
} aresta;

//struct de vertices do grafo 
typedef struct v{
	char nomeEst[64];
	int qtdeArestas;
	aresta *first;
} vertice;

typedef struct c{
	char nomeEst[64];
	int cor;        //0=branco, 1=cinza, 2=preto
	int distancia;
	int valorAnt;
} vert_visita;

typedef aresta *aresta_ptr;

//<<<<<<<<<<<<<< scan_quote_string
void scan_quote_string(char *str) {
	/*
	*	Use essa função para ler um campo string delimitado entre aspas (").
	*	Chame ela na hora que for ler tal campo. Por exemplo:
	*
	*	A entrada está da seguinte forma:
	*		nomeDoCampo "MARIA DA SILVA"
	*
	*	Para ler isso para as strings já alocadas str1 e str2 do seu programa, você faz:
	*		scanf("%s", str1); // Vai salvar nomeDoCampo em str1
	*		scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2 (sem as aspas)
	*
	*/

	char R;

	while((R = getchar()) != EOF && isspace(R)); // ignorar espaços, \r, \n...

	if(R == 'N' || R == 'n') { // campo NULO
		getchar(); getchar(); getchar(); // ignorar o "ULO" de NULO.
		strcpy(str, ""); // copia string vazia
	} else if(R == '\"') {
		if(scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
			strcpy(str, "");
		}
		getchar(); // ignorar aspas fechando
	} else if(R != EOF){ // vc tá tentando ler uma string que não tá entre aspas! Fazer leitura normal %s então, pois deve ser algum inteiro ou algo assim...
		str[0] = R;
		scanf("%s", &str[1]);
	} else { // EOF
		strcpy(str, "");
	}
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

//Printa 1 vertice por vez
void printGrafo(vertice *vert[], int at){
	aresta_ptr atual;
	int i;
	vertice *ver_at = vert[at];

	if(vert[at]->nomeEst != NULL && vert[at]->nomeEst[0] == '\0')
		printf("linha vazia");

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

//Função para ver se arquivo está consistente ou não
int checkFile (FILE *arq){
	char status;
	status=getc(arq);
	//printf("Status: %c\n", status);
	if(status=='1'){
		return 1;   //Arquivo consistente
	}else{
		return 0;   //Arquivo inconsistente
	}
}

//Funcao que pula o cabecalho
void pula_cabecalho(FILE *arq){
	fseek(arq, 17, SEEK_SET);
	return;
}

//Aloca memoria para o vertice atual e add na posicao correta do vetor
int ordenacaoVertice(char estacao[], vertice *vert[]){
	int i = 0;
	int at;
	vertice *temp, *atual;
	
	while(vert[i]!=NULL && strcmp(estacao, vert[i]->nomeEst)>0){
		i++;
	}
	//printf("Ordenacao Vertice: i=%d\n", i);
	//printf("vert[i]:  %p\n", vert[i]);
	//printf("vert[i].nomeEst:  %s\n", vert[i]->nomeEst);
	//printf("estacao:  %s\n", estacao);

	if(vert[i]== NULL){      //Tem que ser inserido na última posição do vetor
		//printf("entra em if(vert[i]==NULL)\n" );
		vert[i]=(vertice*)calloc(1,sizeof(vertice));
		strcpy(vert[i]->nomeEst, estacao);
		//printf("nomeEst_dentro_do_ordenacaoVertice: %s\n", vert[i]->nomeEst);
		vert[i]->qtdeArestas = 1;
		return i;
	}

	if(strcmp(estacao, vert[i]->nomeEst)==0){  //vertice ja existe
		//printf("entra if\n");
		return i;
	}

	//printf("sai if\n");

	//Inserir vertice no meio da lista
	atual=(vertice*)calloc(1,sizeof(vertice));
	//printf("pos calloc\n");
	strcpy(atual->nomeEst, estacao);
	//printf("pos strcpy\n");
	atual->qtdeArestas = 1;
	//printf("pos atual->qtdeArestas\n");
	at = i;
	//printf("i=at: %d\n", i);

	while(vert[i]!= NULL){
		temp = vert[i];
		vert[i] = atual;
		atual = temp;
		i++;
	}

	//printf("temp: %s\n", temp->nomeEst);
	//printf("print i_2: %d \n", i);

	vert[i]=(vertice*)calloc(1,sizeof(vertice));
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
			//printf("Encontrou: vertice procurado - %d, vertice encontrado - %s\n", codEst, nomeEst);
			return 0;
		}
		fseek(arq, (tamReg-12), SEEK_CUR);
	}
	//printf("Nao encontrou codigo buscado\n");
	
	return 1;   //Erro, nao encontrou nome da estacao
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

	while((atual!= NULL) && (strcmp(nomeEst_original, atual->nomeEst)) != 0){
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
void criaAresta(int codProxEst, vertice *vert[], int i, int distancia, char nomeLinha[], FILE *busca){
	char nomeProxEst[64];
	int retorno_erro;
	aresta_ptr insercao, arestaAnterior;

	arestaAnterior=vert[i]->first;
	
	retorno_erro = buscar(codProxEst, busca, nomeProxEst);
	if (retorno_erro==1){
		return;       //Nao encontrou nomeEst com esse codigo, pular
	}
	
	//Encontrou nome da estacao que sera add na aresta
	
	//Aloca memória e insere os dados
	insercao = (aresta_ptr)calloc(1,sizeof(aresta));
	insercao->dist = distancia;
	strcpy(insercao->nomeEst, nomeProxEst);
	memset(insercao->nomeLinha, '\0', sizeof(insercao->nomeLinha));
	strcpy(insercao->nomeLinha[0], nomeLinha);
	insercao->prox = NULL;

	//Aumenta qtde arestas do vertice atual
	vert[i]->qtdeArestas++;

	if(arestaAnterior!=NULL){   //Nao eh a primeira aresta do vertice
		//Ordena a aresta alocada, arrumando os ponteiros de insercao
		ordenacaoAresta(insercao, vert, i, nomeLinha);
	}else{
		vert[i]->first = insercao;
	}

	return;
}

int criaGrafoNaoDirecionado(FILE *arq, FILE *busca, vertice *vert[], int posVert){
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
					/*retorno_erro = buscar(codProxEst, busca, nomeProxEst);
					if (retorno_erro==1){
						return 1;
					}

					//Encontrou nome da estacao que sera add na aresta
					insercao = (aresta_ptr)calloc(1,sizeof(aresta));

					insercao->dist = distancia;
					strcpy(insercao->nomeEst, nomeProxEst);
					memset(insercao->nomeLinha, '\0', sizeof(insercao->nomeLinha));
					strcpy(insercao->nomeLinha[0], nomeLinha);
					insercao->prox = NULL;
					vert[i]->first = insercao;*/
					criaAresta(codProxEst, vert, i, distancia, nomeLinha, busca);
				}else{
						vert[i]->first = NULL;
				}

				if(codEstInt!=-1){  //Existe estacao de integracao
					retorno_erro=aresta_integracao(busca, vert, codEstInt, i);
					/*if(retorno_erro==1){
						return 1;
					}*/
				}

			}else{  //Vértice já existe, só inserir a aresta atual
				if(codProxEst!=-1){         //Se existir nome da linha
					//Funcao que percorre arestas, retorna 0 se nao existe aresta com msm nomeEst e 1 se existe aresta c msm nomeEst
					existeAresta = percorreAresta(nomeProxEst, vert, i);
					if(existeAresta!=NULL){    //Aresta ja existe, so add nova linha
						ordenacaoLinhas(existeAresta, nomeLinha);
					}else{                  //Aresta n existe, precisa ser criada e add corretamente
						//Criacao da aresta!
						criaAresta(codProxEst, vert, i, distancia, nomeLinha, busca);
						/*retorno_erro = buscar(codProxEst, busca, nomeProxEst);
						if (retorno_erro==1){
							return 1;
						}

						//Encontrou nome da estacao que sera add na aresta
						
						//Aloca memória e insere os dados
						insercao = (aresta_ptr)calloc(1,sizeof(aresta));
						insercao->dist = distancia;
						strcpy(insercao->nomeEst, nomeProxEst);
						memset(insercao->nomeLinha, '\0', sizeof(insercao->nomeLinha));
						strcpy(insercao->nomeLinha[0], nomeLinha);
						insercao->prox = NULL;

						//Aumenta qtde arestas do vertice atual
						vert[i]->qtdeArestas++;

						//Ordena a aresta alocada, arrumando os ponteiros de insercao
						ordenacaoAresta(insercao, vert, i, nomeLinha);*/
					}
				}
				if(codEstInt!=-1){  //Existe estacao de integracao
					retorno_erro=aresta_integracao(busca, vert, codEstInt, i);
					/*if(retorno_erro==1){
						return 1;
					}*/
				}
			}
			//printf("\n");
		}
	} 
	return 0;
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
					retorno_erro = buscar(codProxEst, busca, nomeProxEst);
					if (retorno_erro==1){
						return 1;
					}

					//Encontrou nome da estacao que sera add na aresta
					insercao = (aresta_ptr)calloc(1,sizeof(aresta));

					insercao->dist = distancia;
					strcpy(insercao->nomeEst, nomeProxEst);
					memset(insercao->nomeLinha, '\0', sizeof(insercao->nomeLinha));
					strcpy(insercao->nomeLinha[0], nomeLinha);
					insercao->prox = NULL;
					vert[i]->first = insercao;
					//criaAresta(codProxEst, vert, i, distancia, nomeLinha, busca);
				}else{
						vert[i]->first = NULL;
				}

				if(codEstInt!=-1){  //Existe estacao de integracao
					retorno_erro=aresta_integracao(busca, vert, codEstInt, i);
					/*if(retorno_erro==1){
						return 1;
					}*/
				}

			}else{  //Vértice já existe, só inserir a aresta atual
				if(codProxEst!=-1){         //Se existir nome da linha
					//Funcao que percorre arestas, retorna 0 se nao existe aresta com msm nomeEst e 1 se existe aresta c msm nomeEst
					existeAresta = percorreAresta(nomeProxEst, vert, i);
					if(existeAresta!=NULL){    //Aresta ja existe, so add nova linha
						ordenacaoLinhas(existeAresta, nomeLinha);
					}else{                  //Aresta n existe, precisa ser criada e add corretamente
						//Criacao da aresta!
						//criaAresta(codProxEst, vert, i, distancia, nomeLinha, busca);
						retorno_erro = buscar(codProxEst, busca, nomeProxEst);
						if (retorno_erro==1){
							return 1;
						}

						//Encontrou nome da estacao que sera add na aresta
						
						//Aloca memória e insere os dados
						insercao = (aresta_ptr)calloc(1,sizeof(aresta));
						insercao->dist = distancia;
						strcpy(insercao->nomeEst, nomeProxEst);
						memset(insercao->nomeLinha, '\0', sizeof(insercao->nomeLinha));
						strcpy(insercao->nomeLinha[0], nomeLinha);
						insercao->prox = NULL;

						//Aumenta qtde arestas do vertice atual
						vert[i]->qtdeArestas++;

						//Ordena a aresta alocada, arrumando os ponteiros de insercao
						ordenacaoAresta(insercao, vert, i, nomeLinha);
					}
				}
				if(codEstInt!=-1){  //Existe estacao de integracao
					retorno_erro=aresta_integracao(busca, vert, codEstInt, i);
					/*if(retorno_erro==1){
						return 1;
					}*/
				}
			}
			//printf("\n");
		}
	} 
	return 0;
}

//Printa estacoes somente
void printEstacoes(){
	
}

//<<<<<<<<<<< Funções Dijkstra >>>>>>>>>>>>>>>>>>>>>>>>
//char origem[], char destino[], vertice first[], int total_vertices, int total_arestas
// Dijkstra(origem, destino, ver, qtdeEst, qtdePares);
void Dijkstra(char origem[], char destino[], vertice *vet[], int total_vertices, int total_arestas){
	int pos;            //Posicao do vertice atual que o Dijkstra está analisando
	int running,tmp, a;
	int fecha[total_vertices];
	int D[total_vertices];
	char ANT[total_vertices][64];
	int cont = 0;
	int i,j;
	int destPos = percorreVertice(destino, vet);
	aresta_ptr curr_a;
	vertice *curr_v;
	
	memset(ANT,'\0',sizeof(char)*total_vertices);
	memset(D,1000000,sizeof(int)*total_vertices);
	memset(fecha, 0, sizeof(int)*total_vertices);

	//printf("VAL_MAX = %d \n",D[0]);

	pos=percorreVertice(origem, vet);
	//printf("Vertice inicial: %s\n", vet[pos]->nomeEst);

	if(pos==-1){
		printf("Falha percorre Vertice.");
	}

	curr_v = vet[pos];
	D[pos] = 0;     //pos é a posição do vertice inicial

	running = 1;
	curr_a=curr_v->first;

	while(running){ //relaxação
		//printf("POS: %d\n",pos);

		while(curr_a!=NULL){ //Atualiza todas as arestas adjacentes com os menores valores possíveis
			tmp=percorreVertice(curr_a->nomeEst, vet); // tmp recebe a estacao que estamos analizando na aresta
			if(fecha[tmp]==0){  //Vertice está aberto
				if(D[tmp]>(curr_a->dist+D[pos])){
					//printf("changedAresta %d -> %d\n",pos,tmp);
					strcpy(ANT[tmp], vet[pos]->nomeEst);
					D[tmp] = (D[pos]+curr_a->dist);
				}
			}
			curr_a=curr_a->prox;
		}

		//tmp = posicao dos vertices adjacentes
		//pos = posicao do vertice atual
	
		int curr_int = 0;
		for(tmp=0; tmp<total_vertices; tmp++){  //Pula para o proximo vertice
			if(fecha[tmp] == 0 && strcmp(ANT[tmp], "\0")!=0){   //Se vertice adjacente está aberto e ja foi analisado no while anterior
				if(fecha[pos] == 1 || D[curr_int] >= D[tmp]){        //Se vertice atual esta preto ou ?? (n entendi esse ou)
					if(fecha[pos] == 0 && D[curr_int]==D[tmp]){      //Se vertice atual está aberto e 
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
		
		if(fecha[pos]){
			running=0;
		}

		fecha[pos] = 1;
		
        curr_v= vet[pos];
		curr_a=curr_v->first;
	}

	pos = destPos;

	while(pos!=-1 && strcmp(ANT[pos], "\0")!=0){
        cont ++;
		pos=percorreVertice(ANT[pos], vet);
	}

	if(cont==1){	//Nao encontrou estacao
		printf("Não existe caminho entre as estações solicitadas.\n");
		return;
	}

	printf("Numero de estacoes que serao percorridas: %d\n", cont );
	
	printf("Distancia que sera percorrida: %d\n", D[destPos]);

	for(i = cont-1; i >= 0; i--){
		pos = destPos;
		for(j = 0; j < i; j++){
			pos=percorreVertice(ANT[pos], vet);
		}
		printf("%s, ", ANT[pos]);
	}
	
	printf("%s\n", destino);
    
	return;
}

//Percorre grafo em profundidade para encontrar ciclo
void percorreProfundidade(vertice *vert[], char *estacaoOrigem, int totalV){
	int posAt;

	aresta_ptr atual, avanco;
	vert_visita *visitados [TAM_VERTICE];

	posAt = percorreVertice(estacaoOrigem, vert);

	visitados[posAt] = (vert_visita *)calloc(1,sizeof(vert_visita));
	visitados[posAt]->cor = 1;
	visitados[posAt]->distancia=0;
	strcpy(visitados[posAt]->nomeEst, estacaoOrigem);

	atual=vert[posAt]->first;

	while (strcmp(atual->nomeEst, estacaoOrigem)!=0){
		if(atual!=NULL){    //Se existir, é a que iremos avancar pois é a com menor valor de nome
			posAt = percorreVertice(estacaoOrigem, vert);
			
		}
	}
	



}

void Prim (){
	
}

void caminho(){

}


int main(){
	char funcionalidade;
	char in[32];
	int status;
	int qtdeEst, i, retorno_erro;
	int qtdePares = 0;
	char nomeEstacaoOrigem [25];
	char origem [64];
	char nomeEstacaoDestino [25];
	char destino [64];
	vertice *ver[TAM_VERTICE];
	FILE *arq;
	FILE *busca;


	scanf("%c", &funcionalidade);

	//<<<<<<<<<<<<<<<<<<<<<<<<<< CHAMADA FUNCIONALIDADE >>>>>>>>>>>>>>>>>>>>>>>>
	switch (funcionalidade){

		//<<<<<<<<<<<<< funcionalidade [1]
		case '7': //Criar grafo
			scanf("%s",in);
			arq = fopen(in, "rb");
			busca = fopen(in, "rb");

		   // printf("abriu os dois arquivos \n");

			status = checkFile(arq);
			if(status==0){
				printf("Falha na execução da funcionalidade1.\n");
				//printf("aqui status cabecalho\n");
				fclose(arq);
				fclose(busca);
				break;
			}
			fseek(arq, 9, SEEK_SET);
			fread(&qtdeEst, 1, sizeof(int), arq);

		   retorno_erro=criaGrafo(arq, busca, ver, 1);
			if(retorno_erro==1){
				printf("Falha na execução da funcionalidade2.\n");
				return 0;
			}
			
			for(i=0; i<qtdeEst; i++){
				printGrafo(ver, i);
				printf("\n");
			}
			

			fclose(arq);
			fclose(busca);
			break;   

		//<<<<<<<<<<<<< funcionalidade [2]
		case '8': //Criar grafo
			scanf("%s",in);
			arq = fopen(in, "rb");
			busca = fopen(in, "rb");

			
			scanf("%s",nomeEstacaoOrigem);
			scan_quote_string(origem);
			scanf("%s",nomeEstacaoDestino);
			scan_quote_string(destino);
		  
			status = checkFile(arq);
			if(status==0){
				printf("Falha na execução da funcionalidade3.\n");

				fclose(arq);
				fclose(busca);
				break;
			}

			fseek(arq, 9, SEEK_SET);
			fread(&qtdeEst, 1, sizeof(int), arq);  //total de vertices
			fread(&qtdePares, 1, sizeof(int), arq); //total de arestas

		   retorno_erro=criaGrafo(arq, busca, ver, 1);
			if(retorno_erro==1){
				printf("Falha na execução da funcionalidade4.\n");
				return 0;
			}
			
			//char origem[], char destino[], vertice first[], int total_vertices, int total_arestas
			Dijkstra(origem, destino, ver, qtdeEst, qtdePares);
			
			fclose(arq);
			fclose(busca);
			break;

		case '9':

			break;

		/*case '10':
			
			break;*/
			
	} 

	return 0;
}
