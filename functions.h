#ifndef	_FUNCTIONS_H
#define	_FUNCTIONS_H

#include "apue.h"
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX1024 1024
#define MAX100 100
#define MAX50 50
#define MAX20 20
#define MAX10 10
#define MAX5 5
#define M 8

#define BUF_SIZE 4096
#define LISTENQ 10

struct tm formatted_timing;
struct tm timestamp_timing;

typedef struct line{
	long admissao;
	long inicio_triagem;
	long fim_triagem;
	long inicio_medico;
	long fim_medico;
}LINE;

int return_year_formatted_tstamp(char * msg);
int return_year_tstamp(long timestamp);
int number_years(LINE * temp, int N_LINHAS);

#endif	/* _FUNCTIONS_H */
