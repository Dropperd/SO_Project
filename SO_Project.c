#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#define MAX100 100

typedef struct line{
	long admissao;
	long inicio_triagem;
	long fim_triagem;
	long inicio_medico;
	long fim_medico;
}LINE;

int main(int argc, char **argv, char **envp){		// Command Line Arguments
	char temp[MAX100];
	int fds[2],source,destination,wcf;
	int N_LINHAS;
	char buffer[1024],wc[10];
	pipe(fds);
	pid_t pid;
	if (argc < 3){
		perror("Insufficient arguments");
		exit(1);
	}
	int number_pids = atoi(argv[1]);
	int pids[number_pids];
  	source = open (argv[2], O_RDONLY);
	if (source == -1) {
		perror ("Opening Source File");
		exit(1);
	}
	system("wc -l < all_timestamps.csv > /tmp/wc.txt");
	wcf = open("/tmp/wc.txt",O_RDONLY);
	if(wcf == -1) {
        perror ("Opening WC File");
		exit(1);
	}
	read(wcf, wc, sizeof(wc));
	N_LINHAS=atoi(wc);
	destination = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, 0666);
	if (destination == -1) {
        perror ("Opening Destination File");
		exit(1);
	}
	LINE * linhas = (LINE*)calloc(N_LINHAS,sizeof(LINE)); //alocacao de memoria
	FILE *fp=fopen("all_timestamps.csv","r");
	if(fp==NULL){
		puts("erro fp");
		exit(-1);
	}
	fscanf(fp,"%*s",temp);
	printf("LINHAS: %d\n",N_LINHAS);
	LINE * tmp=linhas;	//apontador temporario para nao desconfigurar apontador inicial da struct
	for(int i=0;i<N_LINHAS-1;i++){
		fscanf(fp,"%ld %*[;] %ld %*[;] %ld %*[;] %ld %*[;] %ld %*[\n]",&(tmp->admissao),&(tmp->inicio_triagem),&(tmp->fim_triagem),&(tmp->inicio_medico),&(tmp->fim_medico));
		tmp++;
	}
	tmp=linhas;	//reset no apontador temporario para a struct
	/*LOOP PARA IMPRIMIR TODAS AS OCCORRENCIAS DA STRUCT
	for(int i=0;i<N_LINHAS-1;i++){
		printf("LINHA:%d ||| %ld %ld %ld %ld %ld\n",i+2,tmp->admissao,tmp->inicio_triagem,tmp->fim_triagem,tmp->inicio_medico,tmp->fim_medico);
		tmp++;
	}*/
	for(int i=0;i<number_pids;i++){ //create child processes
		if (pids[i]= fork() == -1){
    		perror("Fork");
    		exit(1);
  		} 
  		int s_admissao=0,s_triagem=0,s_espera=0,s_consulta=0;
		/*if (pids[i] == 0) { 
			for(int j=i;j<N_LINHAS-1-i;j++){
				tmp+=j;
				if()
			}
		}*/
		exit(0); 
	}
}