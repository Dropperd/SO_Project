#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include <ctype.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include<fcntl.h>


int main(int argc, char **argv,char **envp)		// Command Line Arguments
{

int fds[2],source,destination,n;

char buffer[1024];

pipe(fds);


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
  
destination = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, 0666);

	if (destination == -1) {
        perror ("Opening Destination File");
		exit(1);
	}


for(int i=0;i<number_pids;i++) //create child processes
    {
if (pids[i]= fork() == -1){
    perror("Fork");
    exit(1);
  } 


if (pids[i] == 0) { 

while ((n = read (source, buffer, sizeof(buffer))) > 0){

for (char *p = strtok(buffer,";"); p != NULL; p = strtok(NULL, " "))
{
  puts(p);
}

//printf("%d",n);

}

}



exit(0); 
  } 


}

