/*Alunas:   Gabriela Vidal      ->  4277535
			Marianna Karenina   ->  10821144
*/
//SCC0607 - Estrutura de Dados 3
//Segundo Trabalho Prático

#include "Prints.h"
#include "CriaGrafos.h"
#include "Prim.h"
#include "Dijkstra.h"
#include "Buscas.h"
#include "Global.h"

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

//Função para ver se arquivo está consistente ou não
int checkFile (FILE *arq){
	char status;
	status=getc(arq);
	
	if(status=='1'){
		return 1;   //Arquivo consistente
	}else{
		return 0;   //Arquivo inconsistente
	}
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
	aresta_ptr* resultado_prim;

	for (i = 0; i < TAM_VERTICE; i++) {
		ver[i] = NULL;
	}

	scanf("%d ", &funcionalidade);

	//<<<<<<<<<<<<<<<<<<<<<<<<<< CHAMADA FUNCIONALIDADE >>>>>>>>>>>>>>>>>>>>>>>>
	switch (funcionalidade){

		//<<<<<<<<<<<<< funcionalidade [7]
		case 7:
			scanf("%s",in);
			arq = fopen(in, "rb");
			busca = fopen(in, "rb");


			status = checkFile(arq);
			if(status==0){
				printf("Falha na execução da funcionalidade.\n");
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
		case 8:
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

		   retorno_erro=criaGrafo(arq, busca, ver, 1);
			if(retorno_erro==1){
				printf("Falha na execução da funcionalidade.\n");
				return 0;
			}
			
			Dijkstra(origem, destino, ver, qtdeEst);
			
			fclose(arq);
			fclose(busca);
			break;

		//<<<<<<<<<<<<< funcionalidade [9]
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
			
			buscaProfundidade(ver, origem, qtdeEst);
			
			fclose(arq);
			fclose(busca);

			break;

		//<<<<<<<<<<<<< funcionalidade [10]
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

		    retorno_erro=criaGrafo(arq, busca, ver, 1);
			if(retorno_erro==1){
				printf("Falha na execução da funcionalidade.\n");
				return 0;
			}

            GrafoNaoDirecionado(ver, qtdeEst);
			
		
			resultado_prim = Prim(origem, ver, qtdeEst);
			buscaProfundidade_prim(ver, resultado_prim, origem, qtdeEst);
			
			fclose(arq);
			fclose(busca);

			break;
	} 
	return 0;
}
