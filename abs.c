#include "parser.h"
#include "hash.h"
#include "abs.h"

int* initTranslator(char* filename)
{
	int *t = (int*)calloc(INIT_RESULTS, sizeof(int));

	// Creating hash table of commands and their codes
	char commands[NUMBER_OF_COMMANDS][STRING] = COMMANDS;
	printf("Хеш таблица команд:\n");
	Result *result = hashTable(commands, 1);
	DataRecord *commandsTable = (DataRecord*)calloc(1, sizeof(DataRecord));
	commandsTable = result->Table;
	t[0] = result->Data[0];
	t[1] = result->Data[1];

	return t;
}

int main(int argc, char *argv[])
{	
	int *t = (int*)calloc(INIT_RESULTS, sizeof(int));


	printf("Initialization...\n\n");

	t = initTranslator(argv[1]);
	int commands_m = t[1];
	int commands_shift = t[2];


	printf("\n\nWorking...\n\n");

	FILE *fp;
	char str[MAXCHAR];
	char* filename = argv[1];
	fp = fopen(filename, "r");
	int numberOfStrings = 0;
	char** parsedStrings[6];
	
	int n = -1;
	while (fgets(str, MAXCHAR, fp) != NULL)
	{
		char** res = calloc(6, sizeof(char*));
		for (int i = 0; i < 5; i++)
		{	
			res[i] = (char*) calloc(strlen(str), sizeof(char));
		}

		//Parsing assembly string
		if (parseString(str, res))
		{
			++numberOfStrings;
			//printf("Оригинал: '%s'\n", rmSymbs(str, "\n"));
			parsedStrings[++n] = (char**) calloc(6, sizeof(char*));
			parsedStrings[n] = res;
		}
		else
		{
			if (!strcmp(res[0], "ERROR"))
				printf("Error in line #%d ('%s')!\n", numberOfStrings, rmSymbs(str, "\n"));
		}
	}


	// First pass
	for (int i = 0; i < numberOfStrings; i++)
	{
		// ...  your code here ...
	}

	fclose(fp);
	return 0;
}
