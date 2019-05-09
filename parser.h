#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXCHAR 1000
#define LABEL ':'
#define COMMENT ';'

char* rmSymbs(char *string, char *x);

void toLowerCase(char *string);

int parseString(char *string, char* res[]);

void printResult(char* res[]);

