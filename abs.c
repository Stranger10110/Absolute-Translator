#include "parser.h"
#include "hash.h"
#include "abs.h"

int main(int argc, char *argv[])
{
	char commands[32][STRING] = COMMANDS;
	Result *result = hashTable(commands);
	DataRecord *commandsTable = (DataRecord*) calloc(1, sizeof(DataRecord));
	//printf("resTab: %s: %d", result->Table[4].key, result->Table[4].Data);
	commandsTable = result->Table;
	int m = result->Data[0];
	//int m = 1;

	printHashTable(commandsTable, m);
	//printf("%s\n", getKey(commandsTable, "and", m));

	/*FILE *fp;
	char str[MAXCHAR];
	char* filename = argv[1];
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("Невозомжно открыть файл '%s'!\n", filename);
		return 1;
	}

	while (fgets(str, MAXCHAR, fp) != NULL)
	{
		char* res[5];
		for (int i = 0; i < 4; i++)
			res[i] = (char*) calloc(strlen(str), sizeof(char));

		if (parseString(str, res))
		{
			printf("Оригинал: '%s'\n", rmSymbs(str, "\n"));

		}
	}

	fclose(fp);*/
	return 0;
}
