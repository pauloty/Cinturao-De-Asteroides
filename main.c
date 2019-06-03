#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NLIN 30
#define NCOL 100
#define NBORDAL 10
#define NBORDAC 30
#define NTIROS 10
#define TAM_POP 100
#define TAM_DNA 10000
#define MEDIA_FILE "dados2.txt"
#define MELHOR_FILE "dados1.txt"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_BOLD   "\x1b[1m"


typedef struct tiro tiro;
typedef struct nave nave;

struct tiro{
	char lado;
	short int lin, col;
	tiro* prox;
};

typedef struct nave{
	short int lin, col;
	int pontos;
	char perdeu;
	char* moves;
	int nmoves;
}nave;

typedef struct listaT{
	tiro* ini;
	int n;
}listaT;

typedef struct vetN{
	nave** vet;
	int n;
}vetN;

enum{
	DIREITA,
	ESQUERDA,
	BAIXO,
	CIMA,
	PARADO,
	NMOV
};

void press_enter(){
	char nome[10];
	printf("press enter\n");
	fflush(stdout);
	getchar();
	scanf("%[^\n]", nome);
}

void geraArqTempName(char *array , char *filneName){
    char dirTemp[] = "dados";
    system("mkdir -p dados");
    sprintf(array , "%s/%s" , dirTemp , filneName);
}

void libera_nave(nave* n){
	free(n->moves);
	free(n);
}

vetN* criar_vetN(){
	vetN* novo = calloc(1, sizeof(vetN));
	novo->vet = (nave**)malloc(TAM_POP*sizeof(nave*));
	return novo;
}

void inserir_vetN(vetN* l, nave* n){
	l->vet[l->n] = n;
	l->n++;
}

listaT* criar_listaT(){
	listaT* novo = calloc(1, sizeof(listaT));
	return novo;
}

void inserir_listaT(listaT* l, tiro* t){
	t->prox = l->ini;
	l->ini = t;
	l->n++;
}

void liberar_vetN(vetN* naves){
	if(naves==NULL)
		return;
	if(naves->vet!=NULL){
		for(int i=0;i<naves->n;i++){
			if(naves->vet[i]!=NULL)
				libera_nave(naves->vet[i]);
		}
		free(naves->vet);
	}
	free(naves);
}

nave* criar_nave(){
	nave* novo = calloc(1, sizeof(nave));
	novo->lin = NLIN+NBORDAL-1;
	novo->col = NBORDAC + (NCOL/2);
	return novo;
}

void posicionar_nave(nave* n){
	if(n==NULL)
		return;

	n->lin = NLIN+NBORDAL-1;
	n->col = NBORDAC + (NCOL/2);
}

nave* assexuado(nave* mae){
	nave* filho = criar_nave();
	int a;
	int n;
	filho->nmoves=TAM_DNA;
	filho->moves = malloc(TAM_DNA*sizeof(char));
	for(int i=0;i<TAM_DNA;i++){
		filho->moves[i] = mae->moves[i];
	}
	if(mae->pontos/100 > 5)
		n = mae->pontos/100;
	else
		n=5;
	for(int i=0;i<n;i++){
		a = rand()%mae->pontos;
		filho->moves[a]+= rand()%(NMOV-1);
		filho->moves[a]%=NMOV;
	}
	return filho;
}

tiro* criar_tiro(){
	tiro* novo = calloc(1, sizeof(tiro));
	novo->lin = rand()%20;
	novo->lado = rand()%3;
	if(novo->lado==BAIXO){
		novo->col = (rand()%NCOL)+NBORDAC;
	}
	else{
		novo->col = rand()%((NCOL/2)+NBORDAC);
		if(novo->lado==ESQUERDA){
			novo->col+=	(NCOL/2)+NBORDAC;	
		}
	}
	
	return novo;
}

int descer_tiro(tiro* t){
	t->lin++;
	if(t->lado == DIREITA){
		t->col++;
	}
	else if(t->lado==ESQUERDA){
		t->col--;
	}
	if(t->lin>=NLIN+NBORDAL)
		return 1;
	if(t->col<0 || t->col>=NCOL+2*NBORDAC)
		return 1;
	return 0;
}

void descer_tiros(listaT* tiros){
	tiro* aux = tiros->ini;
	tiro* ant;
	int saiu;
	while(aux!=NULL){
		saiu = descer_tiro(aux);
		
		if(saiu){
			if(aux==tiros->ini){
				tiros->ini = tiros->ini->prox;
				free(aux);
				aux=tiros->ini;
				tiros->n--;
			}
			else if(ant!=NULL){
				ant->prox = aux->prox;
				free(aux);
				aux=ant;
				tiros->n--;
			}
		}

		ant = aux;
		if(aux!=NULL)
			aux = aux->prox;	
	}
}

int imprime_tela(listaT* tiros, nave* eu){
	int perdeu=0;
	char nave = 'M';
	char tela[NLIN+NBORDAL][NCOL+2*NBORDAC];
	for(int i=0;i<NLIN+NBORDAL;i++){
		for(int j=0;j<NCOL+2*NBORDAC;j++){
			tela[i][j]=' ';
		}
	}
	tiro* aux = tiros->ini;
	while(aux!=NULL){
		tela[aux->lin][aux->col]='*';
		aux = aux->prox;
	}

	if((tela[eu->lin][eu->col]=='*')
		// || (eu->lin-1>NBORDAL && tela[eu->lin-1][eu->col]=='*')
		// || (eu->lin+1<NBORDAL+NLIN-1 && tela[eu->lin+1][eu->col]=='*')
		// || (eu->col-1>NBORDAC && tela[eu->lin][eu->col-1]=='*')
		// || (eu->col+1<NBORDAC+NCOL-1 && tela[eu->lin][eu->col+1]=='*')
		){
		perdeu =1;
		nave = 'X';
	}

	tela[eu->lin][eu->col] = nave;

	for(int i=NBORDAL;i<NLIN+NBORDAL;i++){
		for(int j=NBORDAC;j<NCOL+NBORDAC;j++){
			if(tela[i][j] == 'M' || tela[i][j]=='X')
				printf(ANSI_COLOR_RED ANSI_COLOR_BOLD "%c" ANSI_COLOR_RESET, tela[i][j]);
			else if(tela[i][j]=='*'){
				printf(ANSI_COLOR_BOLD "%c" ANSI_COLOR_RESET, tela[i][j]);
			}
			else printf(" ");
		}
		printf("\n");
	}
	return perdeu;
}

void print_listaT(listaT* l){
	tiro* aux = l->ini;
	printf("%d\n", l->n);
	while(aux!=NULL){
		printf("%p\n", aux);
		aux = aux->prox;
	}
	printf("\n");
}

vetN* criar_pop(int n){
	vetN* naves = criar_vetN();
	nave* aux;
	for(int i=0;i<n;i++){
		aux = criar_nave();
		aux->nmoves=TAM_DNA;
		aux->moves = malloc(TAM_DNA*sizeof(char));
		for(int j=0;j<TAM_DNA;j++){
			aux->moves[j] = rand()%NMOV;
		}
		inserir_vetN(naves, aux);
	}
	return naves;
}

int verifica_colisoes(listaT* tiros, vetN* naves, int pontos){
	int perdeu=1;
	char tela[NLIN+NBORDAL][NCOL+2*NBORDAC];
	for(int i=0;i<NLIN+NBORDAL;i++){
		for(int j=0;j<NCOL+2*NBORDAC;j++){
			tela[i][j]=' ';
		}
	}
	tiro* aux = tiros->ini;
	while(aux!=NULL){
		tela[aux->lin][aux->col]='*';
		aux = aux->prox;
	}

	nave* atual;
	for(int i=0;i<naves->n;i++){
		atual = naves->vet[i];
		if(atual->perdeu==0){
			atual->pontos = pontos;
			//printf("erro aqui\n");
			fflush(stdout);
			if((tela[atual->lin][atual->col]=='*')
			// || (atual->lin-1>NBORDAL && tela[atual->lin-1][atual->col]=='*')
			// || (atual->lin+1<NBORDAL+NLIN-1 && tela[atual->lin+1][atual->col]=='*')
			// || (atual->col-1>NBORDAC && tela[atual->lin][atual->col-1]=='*')
			// || (atual->col+1<NBORDAC+NCOL-1 && tela[atual->lin][atual->col+1]=='*')
			){
				atual->perdeu=1;
			}
			else{
				perdeu=0;
			}
		}	
	}
	return perdeu;
}

void movimentar_nave(nave* n, int pontos){
	if(pontos>=TAM_DNA)
		return;
	if(n->moves[pontos]==DIREITA && n->col<NBORDAC+NCOL-1){
		n->col++;
	}
	else if(n->moves[pontos]==ESQUERDA && n->col>NBORDAC){
		n->col--;
	}
	else if(n->moves[pontos]==BAIXO && n->lin<NBORDAL+NLIN-1){
		n->lin++;
	}
	else if(n->moves[pontos]==CIMA && n->lin>NBORDAL){
		n->lin--;
	}
}

void movimentar_naves(vetN* naves, int pontos){
	nave* atual;
	for(int i= 0;i<naves->n;i++){
		atual = naves->vet[i];
		if(atual->perdeu==0){
			movimentar_nave(atual, pontos);
		}
	}
}

void avaliar_pop(vetN* naves){
	int perdeu=0;
	unsigned int pontos=0;
	int nt = NTIROS;
	int nivel=0;
	listaT* tiros = criar_listaT();
	tiro* t;
	srand(123);
	while(!perdeu){
		//system("clear");		

		descer_tiros(tiros);
		for(int i=0;i<nt;i++){
			t = criar_tiro();
			inserir_listaT(tiros, t);
		}
			

		pontos++;
		nivel++;
		if(nivel == 500){
			nt++;
			nivel=0;
		}
		perdeu = verifica_colisoes(tiros, naves, pontos);		
		movimentar_naves(naves, pontos);
		perdeu = verifica_colisoes(tiros, naves, pontos);
		//perdeu=0;
		//fflush(stdout);
		//printf("%d\n", pontos);
		//print_listaT(tiros);
		//usleep(TAM_DNA0);

	}
	printf("PERDEU! %d pts %d\n", pontos, nt);

}

nave* trepar(nave* pai, nave* mae){
	nave* filho = criar_nave();
	filho->nmoves=TAM_DNA;
	filho->moves = malloc(pai->nmoves*sizeof(char));
	for(int i=0;i<TAM_DNA;i++){
		if(rand()%2){
			filho->moves[i] = pai->moves[i];
		}
		else{
			filho->moves[i] = mae->moves[i];
		}
		if(!rand()%10){
			filho->moves[i]+= rand()%(NMOV-1);
			filho->moves[i]%=NMOV;
		}
	}
	return filho;
}

void listar_pop(vetN* naves){
	nave* aux;
	printf("%d\n", naves->n);
	for(int i=0;i<naves->n;i++){
		aux = naves->vet[i];
		printf("%d - %d\n", i, aux->pontos);
		// for(int j=0;j<200;j++){
		// 	printf("%d", aux->moves[j]);
		// }
		// printf("\n");
	}
}

int compara_nave(nave** n1, nave** n2){
	if((*n1)->pontos < (*n2)->pontos)
		return 1;
	else if((*n1)->pontos == (*n2)->pontos)
		return 0;
	return -1;
}

int melhor_pontuacao(vetN* naves){
	return naves->vet[0]->pontos;
}

int media_pontuacao(vetN* naves){
	int res=0;
	for(int i=1;i<naves->n;i++){
		res+=naves->vet[i]->pontos;
	}
	res/=(naves->n-1);
	return res;
}

void reproduzir_pop(vetN** naves, int* maior, int* media){
	srand(time(NULL));

	FILE* arq;
    FILE* arq2;
	int n1, n2;
    char fileName1[32];
    char fileName2[32];

	nave *filho, *mae;
	vetN* naves2 = criar_vetN();

	qsort((*naves)->vet, TAM_POP, sizeof(nave*), (int (*)(const void*, const void*)) compara_nave);
    geraArqTempName(fileName1 ,MELHOR_FILE);
    geraArqTempName(fileName2 ,MEDIA_FILE);
	*maior = (*naves)->vet[0]->pontos;
	//printf("o melhor é %d\n" , *maior);
	*media = media_pontuacao(*naves);
	arq = fopen("evolucao", "a");
	if(arq!=NULL){
		fseek(arq, 0, SEEK_END);
		fprintf(arq, "%d\n", *maior);
		fclose(arq);
	}

    arq2 = fopen(fileName1 , "a");
    if(arq2!=NULL){
        fseek(arq2,0,SEEK_END);
        fprintf( arq2,"%d\n" , *maior);
        fclose(arq2);
    }

    arq2 = fopen(fileName2 , "a");
    if(arq2!=NULL){
        fprintf(arq2,"%d\n" , *media);
        fclose(arq2);
    }

	for(int i=0;i<TAM_POP/2;i++){
		n1 = rand()%TAM_POP;
		n2 = rand()%TAM_POP;
		if((*naves)->vet[n1]->pontos > (*naves)->vet[n2]->pontos)
			mae = (*naves)->vet[n1];
		else
			mae = (*naves)->vet[n2];
		filho = assexuado(mae);
		inserir_vetN(naves2, filho);
	}

	for(int i=0;i<TAM_POP/2;i++){
		filho =(*naves)->vet[i];
		if(filho->pontos<TAM_DNA){
			filho->moves[filho->pontos-1]+=rand()%(NMOV-1);
			filho->moves[filho->pontos-1]%=NMOV;
		}
		filho->perdeu = 0;
		posicionar_nave(filho);
		inserir_vetN(naves2, (*naves)->vet[i]);
		(*naves)->vet[i] = NULL;
	}
	//listar_pop(naves2);

	liberar_vetN(*naves);
	*naves = naves2;
}

void salvar_pop(vetN* naves){
	nave* atual;
	FILE* arq = fopen("populacao", "wb+");
	fwrite(&naves->n, 1, sizeof(int), arq);
	int nmoves = naves->vet[0]->nmoves;
	fwrite(&nmoves, 1, sizeof(int), arq);

	for(int i=0;i<naves->n;i++){
		atual = naves->vet[i];
		fwrite(&atual->pontos, 1, sizeof(int), arq);
		fwrite(atual->moves, nmoves, sizeof(char), arq);
	}
	fclose(arq);
}

vetN* recuperar_pop(){
	FILE* arq = fopen("populacao", "rb+");
	if(arq==NULL)
		return NULL;
	int nmoves;
	int npop;
	vetN* naves = criar_vetN();
	nave* aux;
	fread(&npop, 1, sizeof(int), arq);
	fread(&nmoves, 1, sizeof(int), arq);
	for(int i=0;i<npop;i++){
		aux = criar_nave();
		aux->nmoves = nmoves;
		aux->perdeu=0;
		aux->moves = malloc(nmoves*sizeof(char));
		fread(&aux->pontos, 1, sizeof(int), arq);
		for(int j=0;j<nmoves;j++){
			fread(&aux->moves[j], 1, sizeof(char), arq);
		}
		inserir_vetN(naves, aux);
	}
	fclose(arq);
	return naves;
}

void mostrar(nave* n){
	int perdeu=0;
	unsigned int pontos=0;
	int nt = NTIROS;
	int nivel=0;
	
	listaT* tiros = criar_listaT();
	tiro* t;
	posicionar_nave(n);
	srand(123);
	while(!perdeu){
		system("clear");		

		descer_tiros(tiros);
		for(int i=0;i<nt;i++){
			t = criar_tiro();
			inserir_listaT(tiros, t);
		}	
		nivel++;
		if(nivel == 500){
			nt++;
			nivel=0;
		}

		perdeu = imprime_tela(tiros, n);
		usleep(100000);
		if(!perdeu){	
			pontos++;
			movimentar_nave(n, pontos);
			usleep(TAM_DNA);
			system("clear");
			perdeu = imprime_tela(tiros, n);
			//perdeu=0;
		}
		fflush(stdout);
		printf("%d\n", pontos);
		//print_listaT(tiros);
		usleep(100000);

	}
	printf("PERDEU! %d pts\n", pontos);
}

nave* acha_melhor(vetN* naves){
	int maior=-1;
	nave* melhor;
	nave* atual;
	for(int i=0;i<naves->n;i++){
		atual = naves->vet[i];
		if(atual->pontos > maior){
			maior = atual->pontos;
			melhor = atual;
		}
	}
	return melhor;
}

void ordena_pop(vetN** naves){
	qsort((*naves)->vet, TAM_POP, sizeof(nave*), (int (*)(const void*, const void*)) compara_nave);
}

vetN* genocidio(vetN* naves){
	vetN* naves2 = criar_pop(TAM_POP - (naves->n/4));
	ordena_pop(&naves);
	for(int i=0;i<naves->n/4;i++){
		inserir_vetN(naves2, naves->vet[i]);
		naves->vet[i]=NULL;
	}
	liberar_vetN(naves);

	return naves2;
}

void rodar_ciclos(vetN** naves, int n){
	int maior, media;
	int vezes=0;
	int p=0;
	for(int i=0;i<n;i++){
		printf("%d\n", i);
		avaliar_pop(*naves);
		srand(time(NULL));
		//printf("avaliarou\n");
		ordena_pop(naves);
		reproduzir_pop(naves, &maior, &media);
		//printf("reproduziu\n");
		//melhor = acha_melhor(*naves);
		printf("%d %d\n", maior, media);
		if(maior-3 < p && p < maior+3){
			vezes++;
		}
		p=maior;

		if(vezes==50){
			vezes=0;
			*naves = genocidio(*naves);
			//printf("genocidio\n");
		}
	}
}

void gerarGraficos(FILE *pipe){
    FILE *dado1 , *dado2;
    char fileName1[27];
    char fileName2[27];
    geraArqTempName(fileName1 , MELHOR_FILE);
    geraArqTempName(fileName2 , MEDIA_FILE);
    printf("%s %s\n" , fileName1 , fileName2);
    dado1 = fopen(fileName1 , "r");
    if(!dado1){
        perror("falha ao criar o arquivo para o grafico\n");
        return;
    }
    dado2 = fopen(fileName2 , "r");
    if(!dado2){
        perror("falha ao criar o arquivo para o grafico\n");
        fclose(dado1);
        return;
    }
    fprintf(pipe, "plot '%s' title 'Melhor de Todos' with lines, '%s' title 'Media Geral' with lines\n" , fileName1 , fileName2);
    fflush(pipe);
}

int main(){
    FILE *pipe;
	int opcao=0;
	int nciclos;
    int podeGrafico = 1;
	nave* melhor;
	vetN* naves = recuperar_pop();
    pipe = popen("gnuplot -persist" , "w");
    if(pipe == NULL){
        printf("não vai dar para gerar graficos");
        podeGrafico = 1;
    }
    fprintf(pipe, "set title 'Evolução do Algoritmo'\n");
    fprintf(pipe, "set key above\n");
    fprintf(pipe, "set xlabel 'Gerações'\n");
    fprintf(pipe, "set ylabel 'Pontuação\n");
	do{
		system("clear");
		printf("1 - ciclos\n");
		printf("2 - mostrar melhor\n");
		printf("3 - listar pop\n");
		printf("4 - criar pop\n");
		printf("5 - genocidio\n");
        if(podeGrafico)
            printf("6 - graficos\n");
		printf("0 - sair\n");
		scanf("%d", &opcao);
		if(opcao==1){
			printf("Quantos ciclos?\n");
			scanf("%d", &nciclos);
			rodar_ciclos(&naves, nciclos);
		}
		else if(opcao==2){
			melhor = acha_melhor(naves);
			mostrar(melhor);
			posicionar_nave(melhor);
		}
		else if(opcao==3){
			listar_pop(naves);
		}
		else if(opcao==4){
            char file[32];
			liberar_vetN(naves);
            geraArqTempName(file , MEDIA_FILE);
            remove(file);
            geraArqTempName(file , MELHOR_FILE);
            remove(file);
			naves = criar_pop(100);
		}
		else if(opcao==5){
			naves = genocidio(naves);
		}else if(opcao == 6 && podeGrafico){
            gerarGraficos(pipe);
        }
		salvar_pop(naves);
		press_enter();
	}while(opcao!=0);

    pclose(pipe);

	return 0;
}
