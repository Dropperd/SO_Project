#include "functions.h"

char *socket_path = "/tmp/socket";

int main(int argc, char **argv, char **envp){		// Command Line Arguments
	char temp[1],wc[MAX10],filename_str[MAX100],input[MAX100];
	int N_LINHAS,N_LINHAS_STRUCT,wcf;
	int number_pids = atoi(argv[1]);
	int position = atoi(argv[2]);
	strcpy(input,argv[3]);
	sprintf(filename_str,"wc -l < %s > /tmp/wc.txt",input);
	system(filename_str);
	wcf = open("/tmp/wc.txt",O_RDONLY);
	if(wcf == -1) {
		perror ("Opening WC File");
		exit(-1);
	}
	read(wcf, wc, sizeof(wc));
	N_LINHAS=atoi(wc)-1;
	N_LINHAS_STRUCT=N_LINHAS/number_pids;
	close(wcf);
	LINE * linhas = (LINE*)calloc(N_LINHAS_STRUCT,sizeof(LINE)); //alocacao de memoria
	FILE *fp=fopen(input,"r");
	if(fp==NULL){
		puts("erro fp");
		exit(-1);
	}
	fscanf(fp,"%*s",temp);//apenas ignora a primeira linha do ficheiro
	for(int i=0;i<=position;i++){	//salta n linhas inicialmente para comecar a ler na linha correta
		fscanf(fp,"%*s",temp);
	}
	LINE * tmpLines=linhas;	//apontador temporario para nao desconfigurar apontador inicial da struct
	LINE * tmpStamp=linhas;	//apontador temporario para nao desconfigurar apontador inicial da struct
	for(int i=0;i<N_LINHAS_STRUCT;i++){
		fscanf(fp,"%ld %*[;] %ld %*[;] %ld %*[;] %ld %*[;] %ld %*[\n]",&(tmpLines->admissao),&(tmpLines->inicio_triagem),&(tmpLines->fim_triagem),&(tmpLines->inicio_medico),&(tmpLines->fim_medico));
		for(int j=0;j<number_pids-1;j++){
			fscanf(fp,"%*s",temp);
		}
		tmpLines++;
	}
	fclose(fp);
	tmpLines=linhas;	//reset no apontador temporario para a struct
	//LOOP PARA IMPRIMIR TODAS AS OCCORRENCIAS DA STRUCT
	/*for(int i=0;i<N_LINHAS_STRUCT;i++){
		printf("LINHA:%d ||| %ld %ld %ld %ld %ld\n",i,tmpLines->admissao,tmpLines->inicio_triagem,tmpLines->fim_triagem,tmpLines->inicio_medico,tmpLines->fim_medico);
		tmpLines++;
	}
	tmpLines=linhas;	//reset no apontador temporario para a struct*/
	long s_admissao=0,s_triagem=0,s_espera=0,s_consulta=0,timestamp;
	pid_t mypid=getpid();
	for(int j=i;j<N_LINHAS_STRUCT;j++){
		timestamp=tmpStamp->admissao;
		if(timestamp!=9999){ 
			for(int k=0;k<N_LINHAS-1;k++){
				if(tmpLines->admissao < timestamp && timestamp <= tmpLines->inicio_triagem && tmpLines->admissao != 9999 && tmpLines->inicio_triagem != 9999)s_admissao++;
				if(tmpLines->inicio_triagem < timestamp && timestamp <= tmpLines->fim_triagem && tmpLines->inicio_triagem != 9999 && tmpLines->fim_triagem != 9999)s_triagem++;
				if(tmpLines->fim_triagem < timestamp && timestamp <= tmpLines->inicio_medico && tmpLines->fim_triagem != 9999 && tmpLines->inicio_medico != 9999)s_espera++;
				if(tmpLines->inicio_medico < timestamp && timestamp <= tmpLines->fim_medico && tmpLines->inicio_medico != 9999 && tmpLines->fim_medico != 9999)s_consulta++;
				tmpLines++;
			}
			tmpLines=linhas;
			sprintf(buf,"%d$%d,%ld,espera_triagem#%ld\n",mypid,j,timestamp,s_admissao);
			write(destination,buf,strlen(buf));
			sprintf(buf,"%d$%d,%ld,sala_triagem#%ld\n",mypid,j,timestamp,s_triagem);
			write(destination,buf,strlen(buf));
			sprintf(buf,"%d$%d,%ld,sala_espera#%ld\n",mypid,j,timestamp,s_espera);
			write(destination,buf,strlen(buf));
			sprintf(buf,"%d$%d,%ld,sala_consulta#%ld\n",mypid,j,timestamp,s_consulta);
			write(destination,buf,strlen(buf));
			//pid$id,timestamp,sala#ocupação
			s_admissao=s_triagem=s_espera=s_consulta=0;
		}
		tmpStamp++;
	}	
	exit(0);
}