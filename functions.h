#ifndef	_FUNCTIONS_H
#define	_FUNCTIONS_H

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include "apue.h"

#define MAX100 100
#define MAX20 20
#define M 8

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
