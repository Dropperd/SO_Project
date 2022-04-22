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
	char temp[1],buf[100];
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
	pid_t pids[number_pids];
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
	LINE * tmpReset=linhas;	//apontador temporario para nao desconfigurar apontador inicial da struct
	LINE * tmpStamp=linhas;	//apontador temporario para nao desconfigurar apontador inicial da struct
	for(int i=0;i<N_LINHAS-1;i++){
		fscanf(fp,"%ld %*[;] %ld %*[;] %ld %*[;] %ld %*[;] %ld %*[\n]",&(tmpReset->admissao),&(tmpReset->inicio_triagem),&(tmpReset->fim_triagem),&(tmpReset->inicio_medico),&(tmpReset->fim_medico));
		tmpReset++;
	}
	tmpReset=linhas;	//reset no apontador temporario para a struct
	/*LOOP PARA IMPRIMIR TODAS AS OCCORRENCIAS DA STRUCT
	for(int i=0;i<N_LINHAS-1;i++){
		printf("LINHA:%d ||| %ld %ld %ld %ld %ld\n",i+2,tmpReset->admissao,tmpReset->inicio_triagem,tmpReset->fim_triagem,tmpReset->inicio_medico,tmpReset->fim_medico);
		tmpReset++;
	}*/
	for(int i=0;i<number_pids;i++){ //create child processes
		if ((pids[i]=fork())==-1){
			perror("Fork");
			exit(1);
		}
		long s_admissao=0,s_triagem=0,s_espera=0,s_consulta=0,timestamp=linhas->admissao;
		if (pids[i] == 0) { 
			int mypid=getpid();
			for(int k=0;N_LINHAS-1;k++){
				for(int j=i;j<N_LINHAS-1;j+=number_pids){
					tmpReset+=i;
					if(tmpReset->admissao < timestamp && timestamp< tmpReset->inicio_triagem){
						s_admissao++;
					}
					if(tmpReset->inicio_triagem < timestamp && timestamp< tmpReset->fim_triagem){
						s_triagem++;
					}
					if(tmpReset->fim_triagem < timestamp && timestamp< tmpReset->inicio_medico){
						s_espera++;
					}
					if(tmpReset->inicio_medico < timestamp && timestamp< tmpReset->fim_medico){
						s_consulta++;
					}
				}
				tmpReset=linhas;
				sprintf(buf,"%d$%d,%ld,espera_triagem,%ld\n",mypid,k,timestamp,s_admissao);
				write(destination,buf,strlen(buf));
				sprintf(buf,"%d$%d,%ld,sala_triagem,%d\n",mypid,k,timestamp,s_triagem);
				write(destination,buf,strlen(buf));
				sprintf(buf,"%d$%d,%ld,sala_espera,%d\n",mypid,k,timestamp,s_espera);
				write(destination,buf,strlen(buf));
				sprintf(buf,"%d$%d,%ld,sala_consulta,%d\n",mypid,k,timestamp,s_consulta);
				write(destination,buf,strlen(buf));
				//pid$id,timestamp,sala#ocupação 516850
				tmpStamp++;
				timestamp=tmpStamp->admissao;
				s_admissao=s_triagem=s_espera=s_consulta=0;
			}
			close(destination);
			exit(0);
		}
		else{
			wait(NULL);
		}
	}
	return 0;
}