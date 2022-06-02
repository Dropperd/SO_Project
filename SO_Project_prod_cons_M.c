#include "functions.h"

PRODUCT buffer[N];
int prodptr=0, consptr=0;
pthread_mutex_t mutex_prod = PTHREAD_MUTEX_INITIALIZER , mutex_cons = PTHREAD_MUTEX_INITIALIZER;
sem_t can_prod,can_cons;
int N_LINHAS;
int PROD=0, CONS=0 ,N_ANOS=0;
LINE * tmpLines;
LINE * tmpReset;
int condicao_paragem;
int flag_prod;
int flag_cons;
int file_descriptors[MAX100];
int array_anos[MAX100];

PRODUCT calcular(LINE * i, int sala){
	long tstamp=i->admissao;
	PRODUCT newprod;
	long s_admissao=0,s_triagem=0,s_espera=0,s_consulta=0;
	LINE * mytmpLines=tmpReset;
	switch(sala){
		case 0: for(int k=0;k<N_LINHAS;k++){
			if(mytmpLines->admissao < tstamp && tstamp <= mytmpLines->inicio_triagem && mytmpLines->admissao != 9999 && mytmpLines->inicio_triagem != 9999)s_admissao++;
			mytmpLines++;
		}
		newprod.timestamp=tstamp;
		newprod.ocupacao=s_admissao;
		strcpy(newprod.sala,"espera_triagem");
		break;
		case 1: for(int k=0;k<N_LINHAS;k++){
			if(mytmpLines->inicio_triagem < tstamp && tstamp <= mytmpLines->fim_triagem && mytmpLines->inicio_triagem != 9999 && mytmpLines->fim_triagem != 9999)s_triagem++;
			mytmpLines++;
		}
		newprod.timestamp=tstamp;
		newprod.ocupacao=s_triagem;
		strcpy(newprod.sala,"sala_triagem");
		break;
		case 2: for(int k=0;k<N_LINHAS;k++){
			if(mytmpLines->fim_triagem < tstamp && tstamp <= mytmpLines->inicio_medico && mytmpLines->fim_triagem != 9999 && mytmpLines->inicio_medico != 9999)s_espera++;
			mytmpLines++;
		}
		newprod.timestamp=tstamp;
		newprod.ocupacao=s_espera;
		strcpy(newprod.sala,"sala_espera");
		break;
		case 3: for(int k=0;k<N_LINHAS;k++){
			if(mytmpLines->inicio_medico < tstamp && tstamp <= mytmpLines->fim_medico && mytmpLines->inicio_medico != 9999 && mytmpLines->fim_medico != 9999)s_consulta++;
			mytmpLines++;
		}
		newprod.timestamp=tstamp;
		newprod.ocupacao=s_consulta;
		strcpy(newprod.sala,"sala_consulta");
		break;
	}
	s_admissao=0,s_triagem=0,s_espera=0,s_consulta=0;
	mytmpLines=tmpReset;
	return newprod;
}

PRODUCT produce(void * params,int j){
	int i=(int)params;
	LINE * tempForThread = tmpReset;
	PRODUCT return_prod;
	return_prod=calcular((tempForThread+i),j);
	return return_prod;
}

void* producer(void * params){
	while(flag_prod<condicao_paragem){
		for(int j=0;j<N_SALAS;j++){
			PRODUCT item=produce(params,j);
			sem_wait(&can_prod);
			pthread_mutex_lock(&mutex_prod);
				buffer[prodptr]=item;
				prodptr=(prodptr+1)%N;
				flag_prod++;
			pthread_mutex_unlock(&mutex_prod);
			sem_post(&can_cons);
		}
		params+=PROD;
	}
	pthread_exit(0);
}

void consume(PRODUCT p){
	char buf[MAX100];
	sprintf(buf,"%ld,%s#%ld\n",p.timestamp,p.sala,p.ocupacao);
	int ano=0;
	for(int i=0;i<N_ANOS;i++){
		if(return_year_tstamp(p.timestamp)==array_anos[i]){
			ano=i;
			break;
		}
	}
	write(file_descriptors[ano],buf,strlen(buf));
}

void* consumer(){
	while(flag_cons<condicao_paragem){
		PRODUCT item;
		sem_wait(&can_cons);
		pthread_mutex_lock(&mutex_cons);
			item=buffer[consptr];
			consume(item);
			consptr=(consptr+1)%N;
			flag_cons++;
		pthread_mutex_unlock(&mutex_cons);
		sem_post(&can_prod);
	}
	pthread_exit(0);
}

int main(int argc, char **argv, char **envp){
	char temp[1],msg[MAX100],wc[MAX10],buf[MAX100],filename_str[MAX100],input[MAX100],outputname[MAX100];
	int fds[2],wcf,pc;
	if (argc != 4){
		perror("Usage : ./program n_producers n_consumers input");
		exit(-1);
	}
	PROD = atoi(argv[1]);
	CONS = atoi(argv[2]);
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
	LINE * linhas = (LINE*)calloc(N_LINHAS,sizeof(LINE)); //alocacao de memoria
	FILE *fp=fopen(input,"r");
	if(fp==NULL){
		puts("File Pointer is NULL");
		exit(-1);
	}
	fscanf(fp,"%*s",temp);//apenas ignora a primeira linha do ficheiro
	tmpLines=linhas;
	tmpReset=linhas;
	for(int i=0;i<N_LINHAS;i++){
		fscanf(fp,"%ld %*[;] %ld %*[;] %ld %*[;] %ld %*[;] %ld %*[\n]",&(tmpLines->admissao),&(tmpLines->inicio_triagem),&(tmpLines->fim_triagem),&(tmpLines->inicio_medico),&(tmpLines->fim_medico));
		tmpLines++;
	}
	fclose(fp);
	tmpLines=linhas;
	N_ANOS=number_years(tmpLines,N_LINHAS);
	int slots=0,year,flag=0;
	for(int y=0;y<N_LINHAS;y++){
		if(tmpLines->admissao!=9999 && tmpLines->inicio_triagem!=9999 && tmpLines->fim_triagem != 9999 && tmpLines->inicio_medico !=9999 && tmpLines->fim_medico !=9999){
			for(int o=0;o<N_ANOS;o++){
				if((year=return_year_tstamp(tmpLines->admissao))==array_anos[o]){
					flag=1;
					break;
				}
			}
			if(flag==0){
				array_anos[slots]=year;
				slots++;
			}
		}
		flag=0;
		tmpLines++;
	}
	for(int k=0;k<N_ANOS;k++){
		sprintf(outputname,"output_%d.txt",array_anos[k]);
		file_descriptors[k]=open(outputname, O_CREAT | O_TRUNC | O_WRONLY, 0666);
	}
	condicao_paragem=N_LINHAS*N_SALAS;
	flag_cons=0,flag_prod=0;
	sem_init(&can_prod, 0, N);
	sem_init(&can_cons, 0, 0);
	pthread_t consumers[CONS],producers[PROD];
	for(int i=0;i<PROD;i++){
		pthread_create(&producers[i], NULL, producer, (void*)i);
	}
	for(int j=0;j<CONS;j++){
		pthread_create(&consumers[j], NULL, consumer, NULL);
	}
	for(int i=0;i<PROD;i++){
		pthread_join(producers[i], NULL);
	}
	for(int j=0;j<CONS;j++){
		pthread_join(consumers[j], NULL);
	}
	for(int z=0;z<N_ANOS;z++){
		close(file_descriptors[z]);
	}
	return 0;
}

int return_year_formatted_tstamp(char * msg){
	long r_timestamp;
	sscanf(msg,"%*[^,] %*[,] %ld %*[^\n] %*[\n]",&r_timestamp);
	return return_year_tstamp(r_timestamp);
}

int return_year_tstamp(long timestamp){
	timestamp_timing = *localtime(&timestamp);
	return timestamp_timing.tm_year+1900;
}
int number_years(LINE * temp, int N_LINHAS){
	LINE * start = temp;
	int year=0,n_years=0,value=0;
	for(int i=0;i<N_LINHAS;i++){
		if(temp->admissao!=9999 && temp->inicio_triagem!=9999 && temp->fim_triagem != 9999 && temp->inicio_medico !=9999 && temp){
			if((value=return_year_tstamp(temp->admissao))!=year){
				n_years++;
				year=value;
			}
		}
		temp++;
	}
	temp=start;
	return n_years;
}