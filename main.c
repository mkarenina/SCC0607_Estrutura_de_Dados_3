/*Alunas:   Gabriela Vidal      ->  4277535
            Marianna Karenina   ->  10821144
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

typedef aresta *aresta_ptr;

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

//Função para ver se arquivo está consistente ou não
int checkFile (FILE *arq){
    char status;
    status=getc(arq);
    printf("Status: %c\n", status);
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
    printf("Ordenacao Vertice: i=%d\n", i);
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
        //printf("print i: %d \n", i);
        //printf("linha 96 \n");
        temp = vert[i];
        //printf("linha 98 \n");
        vert[i] = atual;
        //printf("linha 100 \n");
        atual = temp;
        //printf("linha 102 \n");
        i++;
    }

    //printf("temp: %s\n", temp->nomeEst);
    //printf("print i_2: %d \n", i);

    vert[i]=(vertice*)calloc(1,sizeof(vertice));
   // temp = atual;
    return at;      //Indica que vertice ja foi inserido no meio da lista
}

//Insere linhaInsercao na posicao ja ordenada da aresta
void ordenacaoLinhas(aresta_ptr atual, char *linhaInsercao){
    int i=0;
    char temp[64];

    while(strcmp(atual->nomeLinha[i], "\0")!=0 && strcmp(linhaInsercao, atual->nomeLinha[i])<0){
        //Enquanto nao chegar no final das linhas ou não encontrar o lugar para inserir
        i++;
    }

    if(strcmp(atual->nomeLinha[i], "\0")==0){
        //Se encontrou lugar no final
        strcpy(atual->nomeLinha[i], linhaInsercao);
        return;
    }

    //Se encontrou lugar no meio
    while(atual->nomeLinha[i] != NULL){
        strcpy(temp, atual->nomeLinha[i]);
        strcpy(atual->nomeLinha[i], linhaInsercao);
        strcpy(linhaInsercao, temp);
        i++;
    }
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
            printf("Encontrou: vertice procurado - %d, vertice encontrado - %s\n", codEst, nomeEst);
            return 0;
        }
        fseek(arq, (tamReg-12), SEEK_CUR);
    }
    printf("Nao encontrou codigo buscado\n");
    
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
    char nomeEstIntegra[64], nomeLinha[12];
    char integracao[20] = "Integração";
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
    strcpy(nomeLinha, integracao);
    atual->prox=NULL;

    ordenacaoAresta(atual, ver, vertice_at, nomeLinha);

    return 0;
}

//Funcao que percorre arestas, retorna 0 se nao existe aresta com msm nomeEst e 1 se existe aresta c msm nomeEst
aresta_ptr percorreAresta (char nomeEst_original[], vertice *origem[], int posAtual){
    aresta_ptr atual = origem[posAtual]->first;

    while(((atual->prox)!= NULL) && (strcmp(nomeEst_original, atual->nomeEst)) != 0){
        atual = atual->prox;
    }

    return atual;
}

//Cria o grafo
int criaGrafo(FILE *arq, FILE *busca, vertice *vert[]){
    int tamRegistro, distancia, i, tamNomeLinha, codProxEst, codEstInt, retorno_erro, tamNomeEst, j;
    char removido;
    char nomeEst[64], nomeLinha[64], nomeProxEst[64];
    aresta_ptr insercao = NULL;
    aresta_ptr existeAresta;

    pula_cabecalho(arq);

    printf("Comecando criaGrafo\n");
    while((removido = getc(arq)) != EOF){
    //for(j=0; j<1; j++){
        //removido = getc(arq);
        fread(&tamRegistro, 1, sizeof(int), arq);
        if(removido==1){    //Se registro foi removido, pular
            printf("Registro removido\n");
            fseek(arq, tamRegistro, SEEK_CUR);
            break;
        }
        
        fseek(arq, 16, SEEK_CUR);   //Chega na distancia até prox estacao, pulei codProxEst pq n faço ideia de como fazer!!
        fread(&codProxEst, 1, sizeof(int), arq);
        fread(&distancia, 1, sizeof(int), arq);
        fseek(arq, 4, SEEK_CUR);    //Pula codLinhaIntegra
        fread(&codEstInt, 1, sizeof(int), arq);
        tamNomeEst=read_field(arq, '|', nomeEst);
        nomeEst[tamNomeEst-1]='\0';
        tamNomeLinha = read_field(arq, '|', nomeLinha);
        nomeLinha[tamNomeLinha-1]='\0';

        printf("Vertice atual: %s\n", nomeEst);

        i=0;
        while(vert[i]!=NULL && strcmp(vert[i]->nomeEst, nomeEst)!=0){
            i++;
        }
        //printf("Sai while, i=%d\n", i);

        if(vert[i]==NULL){      //Insere dado novo vertice
            //printf("Vertice null -> %s\n", nomeEst);
            i = ordenacaoVertice(nomeEst, vert);
            //printf("nomeEst_dentro_do_criaGrafo: %s\n", vert[i]->nomeEst);
            printf("Ordenou, i=%d\n", i);

            if(tamNomeLinha>1){         //Se existir nome da linha
                //Criacao da aresta!
                printf("Aresta atual: %d\n", codProxEst);
                retorno_erro = buscar(codProxEst, busca, nomeProxEst);
                printf("Aresta atual: %s\n", nomeProxEst);
                if (retorno_erro==1){
                    return 1;
                }

                //printf("chaga antes do calloc \n");

                //Encontrou nome da estacao que sera add na aresta
                insercao = (aresta_ptr)calloc(1,sizeof(aresta));

                //printf("chaga depois do calloc \n");
                insercao->dist = distancia;
                //printf("linha 299 \n");
                strcpy(insercao->nomeEst, nomeProxEst);
                //printf("linha 301 \n");
                memset(insercao->nomeLinha, '\0', sizeof(insercao->nomeLinha));
                //printf("linha 303 \n");
                strcpy(insercao->nomeLinha[0], nomeLinha);
                //printf("linha 305 \n");
                insercao->prox = NULL;
                //printf("linha 307 \n");
                vert[i]->first = insercao;
                //printf("linha 309 \n");
            }else{
                //printf("linha 311 \n");
                vert[i]->first = NULL;
                //printf("linha 313 \n");
            }
            //printf("chaga antes do aresta_integração \n");

            printf("CodEstInt = %d\n", codEstInt);
            if(codEstInt!=-1){  //Existe estacao de integracao
                retorno_erro=aresta_integracao(busca, vert, codEstInt, i);
                if(retorno_erro==1){
                    return 1;
                }
            }

        }else{  //Vértice já existe, só inserir a aresta atual
            if(tamNomeLinha>1){         //Se existir nome da linha
                //Funcao que percorre arestas, retorna 0 se nao existe aresta com msm nomeEst e 1 se existe aresta c msm nomeEst
                existeAresta = percorreAresta(nomeProxEst, vert, i);
                if(existeAresta!=NULL){    //Aresta ja existe, so add nova linha
                    ordenacaoLinhas(existeAresta, nomeLinha);
                }else{                  //Aresta n existe, precisa ser criada e add corretamente
                    //Criacao da aresta!
                    printf("Aresta atual: %d\n", codProxEst);
                    retorno_erro = buscar(codProxEst, busca, nomeProxEst);
                    printf("Aresta atual: %s\n", nomeProxEst);
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
                if(retorno_erro==1){
                    return 1;
                }
            }
        }
        printf("\n");
    } 
    return 0;
}

//Printa 1 vertice por vez
void printGrafo(vertice *vert[], int at){
    aresta_ptr atual;
    int i;

    printf("%s ", vert[at]->nomeEst);
    
    atual = vert[at]->first;
    while(atual!=NULL){
        printf("%s %d", atual->nomeEst, atual->dist);
        for(i=0; strcmp(atual->nomeLinha[i], "\0")!=0; i++){
            printf(" %s", atual->nomeLinha[i]);
        }
    }
    return;
}

//Printa estacoes somente
void printEstacoes(){
    
}

/*

bool existeAberto(int *aberto, int total_vertices){
    int i;
    for (i=0; i< total_vertices; i ++){
        if(aberto[i]){
            return(true);
        }
    }
    return(false);
}

int menorDis(total_vertices, int *aberto, int *D){
    int i;
    for(i=0, i< total_vertices; i++){
        if(aberto[i]){
            break;
        }
    }
    if (i == total_vertices){
        return(-1);
    }

    int menor = i;
    for(i = menor+1; i< total_vertices; i++){
        if(aberto[i]&&(D[menor]>D[i])){
            menor = i;
        }
    }
    return(menor);
}

void Dijkstra(char origem[], char destino[], vertice first[], int total_vertices, int total_arestas){

    int *D = (int *)malloc(total_vertices*sizeof(int));
    int ANT[total_vertices];
    bool aberto[total_vertices];
    int u = 0;
    int v;
    int i;
    int j;
    int menor = 0;

    //inicializa Dijkstra
    for (v = 0; v < total_vertices; v++){
        D[v] = INT_MAX/2;
        ANT[v] = -1;
    }

    D[0] = 0; // coloca a origem como 0 de distancia
    
    for (i=0; i < total_vertices; i++){
        aberto[i] = true;                   //entende inicialmente todos os vertices como abertos
    }

    while (existeAberto(aberto, total_vertices)){
                  
            for(j=0, j< total_vertices; j++){
                if(aberto[j]){
                    break;
                }
            }
            
            if (j == total_vertices){
                menor = -1;          //return e vai para linha u=menor;
            }

            if(menor != -1){
                menor = j;
                for(j = menor+1; j< total_vertices; j++){
                    if(aberto[j]&&(D[menor]>D[j])){
                        menor = j;
                    }
                }
             }  
               
        u = menor;
        aberto[u] = false;

        //ADJACENCIA *ad =  g -> adj[u].cad;
        aresta *ad =  first[0].first; //pega o começo da lista de aresta

        while(ad){

                aresta *ad =  first[0].first;
                while (ad && ad -> prox != v){
                    ad = ad->prox;
                }
                if(ad){
                    if(D[v]> D[u]+ad -> dist){
                        D[v] = D[u] + ad -> dist;
                        ANT[v] = u;
                    }
                }
                
            ad = ad->prox;
        }
    }

    return D;
}
*/

void Prim (){
    
}

void caminho(){

}


int main(){
    char funcionalidade;
    char in[32];
    int status;
    int qtdeEst, i, retorno_erro;
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

            printf("abriu os dois arquivos \n");

            status = checkFile(arq);
            if(status==0){
                printf("Falha na execução da funcionalidade1.\n");
                printf("aqui status cabecalho\n");
                fclose(arq);
                fclose(busca);
                break;
            }
            fseek(arq, 9, SEEK_SET);
            fread(&qtdeEst, 1, sizeof(int), arq);

           retorno_erro=criaGrafo(arq, busca, ver);
            if(retorno_erro==1){
                printf("Falha na execução da funcionalidade2.\n");
                return 0;
            }
            printf("Criou grafo\n");
            for(i=0; i<qtdeEst; i++){
                //printGrafo(ver, i);
                //printf("\n");
            }
            

            fclose(arq);
            fclose(busca);
            break;
            
    }    

    return 0;
}
