#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct a {
	char cidade[64];
	char estado [3];
	char tempo [32];
	int dist;
	struct a *nxt;
} elem;

typedef elem *elem_ptr;

void scan_quote_string(char *str) {
	char R;

	while((R = getchar()) != EOF && isspace(R)); // ignorar espa?s, \r, \n...

	if(R == 'N' || R == 'n') { // campo NULO
		getchar(); getchar(); getchar(); // ignorar o "ULO" de NULO.
		strcpy(str, ""); // copia string vazia
	} else if(R == '\"') {
		if(scanf("%[^\"]", str) != 1) { // ler at?o fechamento das aspas
			strcpy(str, "");
		}
		getchar(); // ignorar aspas fechando
	} else if(R != EOF){ // vc t?tentando ler uma string que n? t?entre aspas! Fazer leitura normal %s ent?...
		str[0] = R;
		scanf("%s", &str[1]);
	} else { // EOF
		strcpy(str, "");
	}
}

//Retorna offset de um RRN
int pos_rrn(int rrn){
	return (19+(85*rrn));
}

//Dado o ponteiro para o arquivo, le ate encontra o limitador fornecido ou o fim da linha
int read_field(FILE *arq, char delim, char *buffer){
	char c=' ';
	int i;

	for(i=0;c!=delim && c!='\n';i++){
		fread(&c,1,1,arq);
		buffer[i]=c;
	}

	buffer[i-1]='\0';

	return i;
}

int tam_lista (FILE *arq){
	int tam;

	fseek(arq, 1, SEEK_SET);
	fread(&tam, 1, sizeof(int), arq);
	return tam;
}

//Fun?o que realiza uma busca bin?ia no grafo
int bin_search(elem vet[], char name[], int tam){
	int ret_ini,ret_fim,tmp;

	int ini = 0;
	int fim = tam-1;

	ret_ini = strcmp(name, vet[ini].cidade);
	ret_fim = strcmp(name, vet[fim].cidade);
	if(ret_ini<0 || ret_fim>0){
		return -1;
	}

	if(ret_ini==0){
		return ini;
	}
	if(ret_fim==0){
		return fim;
	}

	while(1){
		if(ret_ini==ret_fim-1){
			return -1;
		}

		tmp = strcmp(name, vet[(ini+fim)/2].cidade);
		if(tmp==0){
			return (ini+fim)/2;
		}else if(tmp>0){
			ini=(ini+fim)/2;
			ret_ini = tmp;
		}else{
			fim=(ini+fim)/2;
			ret_fim = tmp;
		}
	}
}

void print_grafo (elem lista[]){
	int i=0;
	elem_ptr atual;

	while(lista[i].nxt!=NULL){
		//printf("     origem: %s %s\n", lista[i].estado, lista[i].cidade);
		printf("%s %s", lista[i].cidade, lista[i].estado);
		atual=lista[i].nxt;
		while(atual!=NULL){
			//printf("          #%s %s %d %s\n", atual->estado, atual->cidade, atual->dist, atual->tempo);
			printf(" %s %s %d", atual->cidade, atual->estado, atual->dist);
			if(atual->tempo[0]!='\0'){
				printf(" %s", atual->tempo);
			}
			atual=atual->nxt;
		}
		printf("\n");
		i++;
	}
}

void criar_lista (elem lista[], char cid[], char eo[], elem_ptr inserir){
	int i=0;
	elem tmp, atual;
	elem_ptr ptr_at, ptr_ant;


	while(lista[i].nxt!=NULL && strcmp(lista[i].cidade, cid)<0){
			i++;
		}

		if(lista[i].nxt==NULL){ //se for=NULL, ent? essa cidade ainda nao apareceu no grafo
			strcpy(lista[i].cidade, cid);
			strcpy(lista[i].estado, eo);
			lista[i].dist=0;
			lista[i].nxt=inserir;
		}else{

			if(strcmp(lista[i].cidade, cid)==0){ //se for =0, ent? a cidade ?repetida e ?necess?io inserir uma nova liga?o a ela
				ptr_at=lista[i].nxt;
				ptr_ant=NULL;
				while(ptr_at!=NULL && strcmp(ptr_at->cidade, inserir->cidade)<0){
					ptr_ant=ptr_at;
					ptr_at=ptr_at->nxt;
				}

				inserir->nxt=ptr_at;
				if(ptr_ant==NULL){
					lista[i].nxt=inserir;
				}else{
					ptr_ant->nxt=inserir;
				}


			}else{  //ent? quer dizer que ?novo, por? ser?inserido no meio da lista, porque ela est?em ordem alfab?ica
				strcpy(atual.cidade, cid);
				strcpy(atual.estado, eo);
				atual.dist=0;
				atual.nxt=inserir;

				while(lista[i].nxt!=NULL){
					tmp=lista[i];
					lista[i]=atual;
					atual=tmp;
					i++;
				}

				tmp=lista[i];
				lista[i]=atual;
				atual=tmp;
			}
		}
		//print_grafo(lista);
		return;
}

void ler_arquivo (elem lista[], FILE *arq, int total){
	int RRN=0;
	char eo[3], ed[3], co[64], cd[64], temp[30];
	int distan;
	int offset;
	elem_ptr inserir;

	offset = pos_rrn(RRN);
	fseek(arq, offset, SEEK_SET);
	RRN++;

	while(fread(eo, sizeof(char), 2, arq)){
		eo[2]='\0';
		fread(ed, sizeof(char), 2, arq);
		ed[2]='\0';
		fread(&distan, sizeof(int), 1, arq);

		read_field(arq, '|', co);
		read_field(arq, '|', cd);
		read_field(arq, '|', temp);

		//printf("#%s %s %d %s %s %s#\n", eo, ed, distan, co, cd, temp);

		inserir=(elem_ptr)calloc(1, sizeof(elem));
		inserir->nxt=NULL;
		strcpy(inserir->cidade, cd);
		strcpy(inserir->estado, ed);
		inserir->dist=distan;
		strcpy(inserir->tempo, temp);

		criar_lista(lista, co, eo, inserir);

		inserir=(elem_ptr)calloc(1, sizeof(elem));
		inserir->nxt=NULL;
		strcpy(inserir->cidade, co);
		strcpy(inserir->estado, eo);
		inserir->dist=distan;
		strcpy(inserir->tempo, temp);
		criar_lista(lista, cd, ed, inserir);

		offset = pos_rrn(RRN);
		fseek(arq, offset, SEEK_SET);
		RRN++;
	}


	return;
}

void dijkstra(elem vet[],  char name[], int tam){
	int pos,running,tmp;
	int done[tam];
	elem ant[tam];
	elem *curr;
	char eo[2];

	memset(ant,0,sizeof(elem)*tam);
	memset(done,0,sizeof(int)*tam);

	pos=bin_search(vet,name,tam);

	if(pos==-1){
		printf("Cidade inexistente.");
	}

	curr = &vet[pos];
	ant[pos] = vet[pos];
	ant[pos].dist = 0;
	done[pos] = 1;

	strcpy(eo,ant[pos].estado);

	running = 1;

	while(running){
		curr=curr->nxt;
		while(curr!=NULL){  //relaxação
			tmp=bin_search(vet,curr->cidade,tam);  
			if(done[tmp]==0){
				if(ant[tmp].dist==0 || ant[tmp].dist>(curr->dist+ant[pos].dist)){
					strcpy(ant[tmp].cidade, vet[pos].cidade);
					strcpy(ant[tmp].estado, vet[pos].estado);
					strcpy(ant[tmp].tempo, vet[pos].tempo);
					ant[tmp].dist = (ant[pos].dist+curr->dist);
				}
			}
			curr=curr->nxt;
		}

		curr=&ant[0];
		for(tmp=0;tmp<tam;tmp++){
			if(done[tmp] == 0){
				if(done[pos] == 1 || curr->dist > ant[tmp].dist){
					curr = &ant[tmp];
					pos = tmp;
				}
			}
		}
		if(done[pos]){
			running=0;
		}

		done[pos] = 1;
		curr=&vet[pos];
	}

	for(pos=0;pos<tam;pos++){
		if(ant[pos].dist>0){
			printf("%s %s %s %s %d %s %s\n",name, eo, vet[pos].cidade, vet[pos].estado, ant[pos].dist, ant[pos].cidade, ant[pos].estado);
		}
	}
}

void tirar_lista (elem vet[], char nome[], int tam, int vertices[]){    //Tira todas as conex�es da lista com o nome
    int i;
    elem_ptr atual, ant;

    for(i=0; vertices[i]!=-1; i++){
        atual=vet[vertices[i]].nxt;
        ant=NULL;
        while(atual!=NULL){
            if(strcmp(atual->cidade, nome)==0){
                printf("tirando cidade %s de %s\n", atual->cidade, vet[vertices[i]].cidade);
                if(ant==NULL){
                    vet[i].nxt=atual->nxt;
                }else{
                    ant->nxt=atual->nxt;
                }
                atual=NULL;
            }
            ant=atual;
            if(atual!=NULL){
                atual=atual->nxt;
            }
        }
    }
}

void prim (elem lista[], char cid[], int tam, elem lista_prim[]){
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
}

int main(){
	FILE *arq;
	char comm[2], situacao;
	char out[32];
	char in[32];
	elem_ptr lista;
	int tam, i;

	scanf("%s", comm);

	if(strcmp(comm, "9")==0){
		scanf("%s",in);
		arq = fopen(in, "r");
		if(arq==NULL){
			printf("Falha no carregamento do arquivo.");
			return 0;
		}
		fread(&situacao, 1, sizeof(char), arq);
		if(situacao=='0'){
			printf("Falha no carregamento do arquivo.");
			return 0;
		}

		tam = tam_lista(arq);

		lista=(elem_ptr)calloc(tam,sizeof(elem));
		for(i=0; i<=tam; i++){
			lista[i].nxt=NULL;
			lista[i].cidade[0]='\0';
		}
		ler_arquivo(lista, arq, tam);

		print_grafo(lista);
	}

	if(strcmp(comm, "10")==0){
        scanf("%s",in);
        scanf("%s", out);
        scan_quote_string(out);
		arq = fopen(in, "r");
		if(arq==NULL){
			printf("Falha no carregamento do arquivo.");
			return 0;
		}
		fread(&situacao, 1, sizeof(char), arq);
		if(situacao=='0'){
			printf("Falha no carregamento do arquivo.");
			return 0;
		}

		tam = tam_lista(arq);

		lista=(elem*)calloc(tam,sizeof(elem));
		for(i=0; i<=tam; i++){
			lista[i].nxt=NULL;
			lista[i].cidade[0]='\0';
		}
		ler_arquivo(lista, arq, tam);
        dijkstra(lista, out, tam);
	}

	if(strcmp(comm, "11")==0){
		scanf("%s",in);
		scanf("%s",out);
		scan_quote_string(out);
		arq = fopen(in, "r");
		if(arq==NULL){
			printf("Falha no carregamento do arquivo.");
			return 0;
		}

		fread(&situacao, 1, sizeof(char), arq);
		if(situacao=='0'){
			printf("Falha no carregamento do arquivo.");
			return 0;
		}

		tam = tam_lista(arq);

		elem_ptr lista_prim;

		lista=(elem_ptr)calloc(tam,sizeof(elem));
		lista_prim=(elem_ptr)calloc(tam,sizeof(elem));
		for(i=0; i<=tam; i++){
			lista[i].nxt=NULL;
			lista[i].cidade[0]='\0';
		}




		ler_arquivo(lista, arq, tam);

		for(i=0; i<tam; i++){
            lista_prim[i]=lista[i];
            lista_prim[i].nxt=NULL;
        }

		prim(lista, out, tam, lista_prim);

		return 0;
	}

	return 0;
}
