#include "parser.h"
#include "hash.h"
#include "abs.h"

void firstPass(char** parsedStrings[6], int numberOfStrings, DataRecord *labelsTable)
{	
	int placeCounter = 1000;

	for (int i = 0; i < numberOfStrings; i++)
	{	
		for (int k = 0; k < 5; k++)
		{
			if (strlen(parsedStrings[i][k]) != 0)
				switch (k)
				{
				case 0: // метка
					break;

				case 2: // операнд 1
					break;

				case 3: // операнд 2
					break;
				}
		}
	}
}

int main(int argc, char *argv[])
{	
	printf("Initialization...\n\n");

	// Creating hash table of commands and their codes
	printf("Хеш таблица команд:\n");
	char commands[NUMBER_OF_COMMANDS][STRING] = COMMANDS;
	Result *result = hashTable(commands, NUMBER_OF_COMMANDS, 1);
	DataRecord *commandsTable = result->Table;
	int com_m = result->Data[0];
	int com_shift = result->Data[1];


	printf("\n\nWorking...\n\n");

	//Parsing assembly strings
	int errors = 0;
	FILE *fp = fopen(argv[1], "r");
	char str[MAXCHAR];
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
	if (errors) return 0;


	printf("\nFirst pass: \n\n");

	char *t[1] = {""};
	DataRecord *labelsTable = hashTable(t, 1, 1);
	firstPass(parsedStrings, numberOfStrings, labelsTable);

	fclose(fp);
	return 0;
}
