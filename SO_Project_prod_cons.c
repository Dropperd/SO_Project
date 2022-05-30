#include "functions.h"

PRODUCT buffer[N],PRODUCER_N=0;
int prodptr=0, consptr=0;
pthread_mutex_t mutex_prod = PTHREAD_MUTEX_INITIALIZER , mutex_cons = PTHREAD_MUTEX_INITIALIZER;
sem_t can_prod,can_cons;

void* producer(void * params){
	while(1){
		PRODUCT item=produce(params);
		sem_wait(&can_prod);
		pthread_mutex_lock(&mutex_prod);
			buffer[prodptr]=item;
			prodptr=(prodptr+1)%N;
		pthread_mutex_unlock(&mutex_prod);
		sem_post(&can_cons);
	}
}

PRODUCT produce(void * params){
	long i=(long)params;
	for(j=0;j<N_SALAS;j++){	
		PRODUCT return_prod=calcular((*(tempForThread+i)),j);
	}
	i+=PROD;
	return return_prod;
}

PRODUCT calcular(LINE * i, int sala){
	long tstamp=i.admissao;
	PRODUCT * newprod = (PRODUCT*)malloc(sizeof(PRODUCT));
	switch(sala){
		case 0: for(int k=0;k<N_LINHAS;k++){
			if(tmpLines->admissao < tstamp && tstamp <= tmpLines->inicio_triagem && tmpLines->admissao != 9999 && tmpLines->inicio_triagem != 9999)s_admissao++;
			tmpLines++;
		}
		newprod.timestamp=tstamp;
		newprod.ocupacao=s_admissao;
		strcpy(newprod.sala,"espera_triagem");
		break;
		case 1: for(int k=0;k<N_LINHAS;k++){
			if(tmpLines->inicio_triagem < tstamp && tstamp <= tmpLines->fim_triagem && tmpLines->inicio_triagem != 9999 && tmpLines->fim_triagem != 9999)s_triagem++;
			tmpLines++;
		}
		newprod.timestamp=tstamp;
		newprod.ocupacao=s_triagem;
		strcpy(newprod.sala,"sala_triagem");
		break;
		case 2: for(int k=0;k<N_LINHAS;k++){
			if(tmpLines->fim_triagem < tstamp && tstamp <= tmpLines->inicio_medico && tmpLines->fim_triagem != 9999 && tmpLines->inicio_medico != 9999)s_espera++;
			tmpLines++;
		}
		newprod.timestamp=tstamp;
		newprod.ocupacao=s_espera;
		strcpy(newprod.sala,"sala_espera");
		break;
		case 3: for(int k=0;k<N_LINHAS;k++){
			if(tmpLines->inicio_medico < tstamp && tstamp <= tmpLines->fim_medico && tmpLines->inicio_medico != 9999 && tmpLines->fim_medico != 9999)s_consulta++;
			tmpLines++;
		}
		newprod.timestamp=tstamp;
		newprod.ocupacao=s_consulta;
		strcpy(newprod.sala,"sala_consulta");
		break;
	}
	tmpLines=linhas;
	return newprod;
}

void* consumer(){
	while(1){
		int item;
		sem_wait(&can_cons);
		pthread_mutex_lock(&mutex_cons);
			item=buffer[consptr];
			buffer[consptr]=-1;
			consptr=(consptr+1)%N;
		pthread_mutex_unlock(&mutex_cons);
		sem_post(&can_prod);
	}
}

int main(int argc, char **argv, char **envp){
	char temp[1],msg[MAX100],wc[MAX10],buf[MAX100],filename_str[MAX100],input[MAX100];
	int fds[2],N_LINHAS,N_ANOS,destination,wcf,pc;
	if (argc != 5){
		perror("Usage : ./program n_producers n_consumers input output");
		exit(-1);
	}
	int PROD = atoi(argv[1]),CONS = atoi(argv[2]);
	strcpy(input,argv[3]);
	sprintf(filename_str,"wc -l < %s > /tmp/wc.txt",input);
	system(filename_str);
	wcf = open("/tmp/wc.txt",O_RDONLY);
	if(wcf == -1) {
		perror ("Opening WC File");
		exit(-1);
	}
	read(wcf, wc, sizeof(wc));
	N_LINHAS=atoi(wc)-1;				//discard first line
	close(wcf);
	destination = open(argv[4], O_CREAT | O_TRUNC | O_WRONLY, 0666);
	if (destination == -1) {
		perror ("Opening Destination File");
		exit(-1);
	}
	LINE * linhas = (LINE*)calloc(N_LINHAS,sizeof(LINE)); //alocacao de memoria
	FILE *fp=fopen(input,"r");
	if(fp==NULL){
		puts("File Pointer is NULL");
		exit(-1);
	}
	fscanf(fp,"%*s",temp);//apenas ignora a primeira linha do ficheiro
	LINE * tmpLines=linhas;	//apontador temporario para nao desconfigurar apontador inicial da struct
	LINE * tmpStamp=linhas;	//apontador temporario para nao desconfigurar apontador inicial da struct
	for(int i=0;i<N_LINHAS;i++){
		fscanf(fp,"%ld %*[;] %ld %*[;] %ld %*[;] %ld %*[;] %ld %*[\n]",&(tmpLines->admissao),&(tmpLines->inicio_triagem),&(tmpLines->fim_triagem),&(tmpLines->inicio_medico),&(tmpLines->fim_medico));
		tmpLines++;
	}
	fclose(fp);
	tmpLines=linhas;
	tempForThread=linhas;
	long s_admissao=0,s_triagem=0,s_espera=0,s_consulta=0,timestamp;
	sem_init(&can_prod, 0, N);
	sem_init(&can_cons, 0, 0);
	pthread_t consumers[CONS],producers[PROD];
	for(int i=0;i<PROD;i++){
		PRODUCER_N++;
		pthread_create(&producers[i], NULL, producer, (void*)i);
	}
	for(int j=0;j<CONS;j++){
		pthread_create(&consumers[i], NULL, consumer, NULL);
	}
	while(1){
		sleep(1);
		pthread_mutex_lock(&mutex_prod);
		pthread_mutex_lock(&mutex_cons);
		for(int i=0;i<N;i++){
			printf("%d ",buffer[i]);
		}
		pthread_mutex_unlock(&mutex_prod);
		pthread_mutex_unlock(&mutex_cons);
		printf("\n");
	}
	return 0;
}