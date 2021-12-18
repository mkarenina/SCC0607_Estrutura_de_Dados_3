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
	char ANT[64];
	int cor;        //0=branco, 1=cinza, 2=preto
	int D;
	//int valorAnt;
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

int ConexaoVertices(vertice vertOrigem, vertice VertDest){
	aresta_ptr atual = vertOrigem->first;

	while(atual!=NULL){
		if(strcmp(atual->nomeEst, VertDest.nomeEst)==0){
			return 1;
		}
		atual=atual->prox;
	}
	return 0;
}

//Funcao para funcionalidade 10
void GrafoNaoDirecionado(vertice *vert[], int qtdeVert){
	int i, existeConec, posVert;
	vertice vertAt;
	aresta_ptr atual;

	for(i=0; i<qtdeVert; i++){
		vertAt=vert[i];
		atual=vertAt.first;
		while(atual!=NULL){
			posVert=percorreVertice(atual->nomeEst, vert);
			existeConec=ConexaoVertices(vert[posVert], vertAt);
			if(!existeConec){

			}
		}
		
	}
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
						//criaAresta(codProxEst, vert, i, distancia, nomeLinha, busca);
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

//Printa estacoes somente
void printEstacoes(){
	
}

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
		//printf("Visitado: %d\n", visitado[pos]->D);
		pos=percorreVertice(visitado[pos]->ANT, vet);
		//if(strcmp(visitado[pos]->ANT, destino)==0 || strcmp(visitado[pos]->ANT, origem)==0){
		
	}

	cont ++;
	Dtot+=visitado[pos]->D;
	//printf("Visitado: %d\n", visitado[pos]->D);
	pos=percorreVertice(visitado[pos]->ANT, vet);

	if(conectado==0 || conectado==-1){	//Nao encontrou estacao
		printf("%s\n", printErro);
		return;
	}

	/*if(D[pos]== cont || cont==0){	//Nao encontrou estacao
		printf("Não existe caminho entre as estações solicitadas.\n");
		return;
	}*/

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

//Dijkstra que percorre o vetor até achar menor caminho entre origem e destino
void Dijkstra(char origem[], char destino[], vertice *vet[], int total_vertices, int total_arestas){
	int pos;            //Posicao do vertice atual que o Dijkstra está analisando
	int running,tmp, a, curr_int;
	int cont = 0;
	int i;
	//int i,j;
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
		//printf("Falha percorre Vertice.");
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
					//printf("changedAresta %d -> %d\n",pos,tmp);
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
    
	return;
}

int recBuscaProfundidade(vertice *vert[], char *origem, vert_visita *visitado[], int posAt){
	int posTeste, respostaRec;
	aresta_ptr curr_a = vert[posAt]->first;
	aresta_ptr avanco = NULL;
	
	while(curr_a!=NULL){	//Percorre as arestas para pegar a de menor caminho
		if(strcmp(curr_a->nomeEst, origem)==0){	//Encontrou ciclo!
			posTeste = percorreVertice(curr_a->nomeEst, vert);
			strcpy(visitado[posTeste]->ANT, vert[posAt]->nomeEst);	//Atualizando struct visitado
			visitado[posTeste]->cor = 1;
			visitado[posTeste]->D = curr_a->dist;
			return posTeste;
		}
		posTeste = percorreVertice(curr_a->nomeEst, vert);
		if(visitado[posTeste]->cor==0){		//Se o vertice ainda nao foi analisado, vai para ele
			avanco=curr_a;
			strcpy(visitado[posTeste]->ANT, vert[posAt]->nomeEst);	//Atualizando struct visitado
			visitado[posTeste]->cor = 1;
			visitado[posTeste]->D = avanco->dist;

			respostaRec = recBuscaProfundidade(vert, origem, visitado, posTeste);
			if(respostaRec!=-1){		//Encontrou ciclo
				//printf("Encontrou ciclo!\n");
				return respostaRec;
			}
		}
		curr_a=curr_a->prox;
	}

	return -1;

		
	// while(curr_a!=NULL){	//Percorre as arestas para pegar a de menor caminho
	// 	if(strcmp(curr_a->nomeEst, origem)==0){	//Encontrou ciclo!
	// 		posTeste = percorreVertice(curr_a, vert);
	// 		return posTeste;
	// 	}
	// 	if(avanco==NULL || curr_a->dist<avanco->dist){	//Se encontra distancia menor entre as arestas do vertice atual
	// 		posTeste = percorreVertice(curr_a->nomeEst, vert);
	// 		if(visitado[posTeste]->cor==0){		//Se o vertice ainda nao foi analisado, vai para ele
	// 			avanco=curr_a;
	// 		}
	// 	}
	// 	if(avanco==NULL || (curr_a->dist==avanco->dist && strcmp(curr_a->nomeEst, avanco->nomeEst))>1){	//Se é a mesma distancia, mas nome da aresta de avanco<atual
	// 		posTeste = percorreVertice(curr_a->nomeEst, vert);
	// 		if(visitado[posTeste]->cor==0){		//Se o vertice ainda nao foi analisado, vai para ele
	// 			avanco=curr_a;
	// 		}
	// 	}
	// 	curr_a=curr_a->prox;
	// }

	/*if(avanco==NULL){	//Todas as arestas desse vertice ja foram analisadas
		return -1;
	}

	//Se chegou aqui entao vai analisar a proxima aresta
	posTeste = percorreVertice(avanco->nomeEst, vert);
	strcpy(visitado[posTeste]->ANT, vert[posAt]->nomeEst);	//Atualizando struct visitado
	visitado[posTeste]->cor = 1;
	visitado[posTeste]->D = avanco->dist;

	respostaRec = recBuscaProfundidade(vert, origem, visitado, posTeste);

	if(respostaRec!=-1){		//Encontrou ciclo
		return respostaRec;
	}*/

	//Nao encontrou ciclo nesse caminho, seguir para o proximo vertice

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
		//printf("Nao encontrou vertice\n");
		return;
	}
	//visitado[posAt]=(vert_visita *)calloc(1,sizeof(vert_visita));
	visitado[posAt]->D=0;
	visitado[posAt]->cor=1;

	curr_a=vert[posAt]->first;
	avanco = curr_a;

	//printf("Entrando recursao\n");
	resposta = recBuscaProfundidade(vert, estacaoOrigem, visitado, posAt);

	//printf("Saiu recursao\n");
	printBusca(estacaoOrigem, "Nao existe um ciclo a partir da estacao de origem.", resposta, estacaoOrigem, vert, resposta, visitado);
	
	return;

}

/*void Prim (char origem[], vertice *vet[], int total_vertices, int total_arestas){
    int i, j=0,k, max;
    elem_ptr atual=NULL, inserir_prim;
    int vertices_usados[tam];
    char cidat[64];

    memset(vertices_usados, -1, sizeof(int)*tam);



    while(j<2){       //enquanto n�o s�o analisados todos os v�rtices
        for(i=0;i<tam;i++){
            if(strcmp(lista[i].cidade, cid)==0){
                break;
            }
        }
        if(strcmp(lista[i].cidade, cid)!=0){
            printf("Cidade inexistente.");
            return;
        }

        vertices_usados[j]=i;

        for(k=0; vertices_usados[k]!=-1; k++){
            //tirar_lista(lista, lista[vertices_usados[k]].cidade, tam, vertices_usados);
        }

        printf("verticeusado: %s\n", lista[i].cidade);
        j++;
        max=99999;

        for(k=0; vertices_usados[k]!=-1; k++){  //enquanto a an�lise dos elementos do vetor de v�rtices usados n�o acabar
            i=vertices_usados[k];

            atual=lista[i].nxt;

            while(atual!=NULL){     //enquanto n�o acaba a an�lise de todos os componentes daquela lista encadeada
                if(max>atual->dist){
                    max=atual->dist;
                    inserir_prim=atual;
                    inserir_prim->nxt=NULL;
                    strcpy(cidat, lista[i].cidade);
                }
                atual=atual->nxt;
            }

        }

        criar_lista(lista_prim, lista[i].cidade, lista[i].estado, inserir_prim);
        //tirar_lista(lista, cid, tam, vertices_usados);

        strcpy(cid, inserir_prim->cidade);
        printf("%s\n", cid);
        print_grafo(lista_prim);
        printf("\n");
        print_grafo(lista);
    }

    print_grafo(lista_prim);

}*/

void caminho(){

}


int main(){
	int funcionalidade;
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


	scanf("%d ", &funcionalidade);

	printf("Funcionalidade:%d \n", funcionalidade);
	//<<<<<<<<<<<<<<<<<<<<<<<<<< CHAMADA FUNCIONALIDADE >>>>>>>>>>>>>>>>>>>>>>>>
	switch (funcionalidade){

		//<<<<<<<<<<<<< funcionalidade [7]
		case 7: //Criar grafo
			scanf("%s",in);
			arq = fopen(in, "rb");
			busca = fopen(in, "rb");

		   // printf("abriu os dois arquivos \n");

			status = checkFile(arq);
			if(status==0){
				printf("Falha na execução da funcionalidade.\n");
				//printf("aqui status cabecalho\n");
				fclose(arq);
				fclose(busca);
				break;
			}
			fseek(arq, 9, SEEK_SET);
			fread(&qtdeEst, 1, sizeof(int), arq);

		   retorno_erro=criaGrafo(arq, busca, ver, 1);
			if(retorno_erro==1){
				printf("Falha na execução da funcionalidade.\n");
				return 0;
			}
			
			for(i=0; i<qtdeEst; i++){
				printGrafo(ver, i);
				printf("\n");
			}
			

			fclose(arq);
			fclose(busca);
			break;   

		//<<<<<<<<<<<<< funcionalidade [8]
		case 8: //Criar grafo
			scanf("%s",in);
			arq = fopen(in, "rb");
			busca = fopen(in, "rb");
			
			scanf("%s",nomeEstacaoOrigem);
			scan_quote_string(origem);
			scanf("%s",nomeEstacaoDestino);
			scan_quote_string(destino);
		  
			status = checkFile(arq);
			if(status==0){
				printf("Falha na execução da funcionalidade.\n");

				fclose(arq);
				fclose(busca);
				break;
			}

			fseek(arq, 9, SEEK_SET);
			fread(&qtdeEst, 1, sizeof(int), arq);  //total de vertices
			fread(&qtdePares, 1, sizeof(int), arq); //total de arestas

		   retorno_erro=criaGrafo(arq, busca, ver, 1);
			if(retorno_erro==1){
				printf("Falha na execução da funcionalidade.\n");
				return 0;
			}
			
			//char origem[], char destino[], vertice first[], int total_vertices, int total_arestas
			Dijkstra(origem, destino, ver, qtdeEst, qtdePares);
			
			fclose(arq);
			fclose(busca);
			break;

		case 9:
			scanf("%s",in);
			arq = fopen(in, "rb");
			busca = fopen(in, "rb");

			scanf("%s",nomeEstacaoOrigem);
			scan_quote_string(origem);
		  
			status = checkFile(arq);
			if(status==0){
				printf("Falha na execução da funcionalidade.\n");

				fclose(arq);
				fclose(busca);
				break;
			}

			fseek(arq, 9, SEEK_SET);
			fread(&qtdeEst, 1, sizeof(int), arq);  //total de vertices

		   retorno_erro=criaGrafo(arq, busca, ver, 1);
			if(retorno_erro==1){
				printf("Falha na execução da funcionalidade.\n");
				return 0;
			}
			
			//char origem[], char destino[], vertice first[], int total_vertices, int total_arestas
			//printf("Entrou buscaProf\n");
			buscaProfundidade(ver, origem, qtdeEst);
			
			fclose(arq);
			fclose(busca);

			break;

		case 10:
			scanf("%s",in);
			arq = fopen(in, "rb");
			busca = fopen(in, "rb");

			scanf("%s",nomeEstacaoOrigem);
			scan_quote_string(origem);
		  
			status = checkFile(arq);
			if(status==0){
				printf("Falha na execução da funcionalidade.\n");

				fclose(arq);
				fclose(busca);
				break;
			}

			fseek(arq, 9, SEEK_SET);
			fread(&qtdeEst, 1, sizeof(int), arq);  //total de vertices

		   retorno_erro=criaGrafoNaoDirecionado(arq, busca, ver, 1);
			if(retorno_erro==1){
				printf("Falha na execução da funcionalidade.\n");
				return 0;
			}
			
			for(i=0; i<qtdeEst; i++){
				printGrafo(ver, i);
				printf("\n");
			}

			//Prim();
			
			fclose(arq);
			fclose(busca);

			break;
			
			break;
			
	} 

	return 0;
}
