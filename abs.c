#include "parser.h"
#include "hash.h"
#include "abs.h"

Result* firstPass(char** parsedStrings[6], int numberOfStrings, DataRecord *registersTable, int r_m, int r_s, DataRecord *labelsTable, int l_m, int l_s)
{	
	int placeCounter = 0;

	int l = -1;
	char** labels = (char**) calloc(numberOfStrings, sizeof(char*));
	int *labelsData = (int*) calloc(numberOfStrings, sizeof(int));

	for (int i = 0; i < numberOfStrings; i++)
	{
		for (int k = 0; k < 5; k++)
		{	
			if (strlen(parsedStrings[i][k]) != 0)
			{
				switch (k)
				{
				case 0: // метка
					/* если метки нет в таблице, заносим её туда и назначаем текущий адрес размещений */
					if (!isKeyOf(labelsTable, parsedStrings[i][0], l_m, l_s))
					{	
						labels[++l] = (char*) calloc(strlen(parsedStrings[i][0]) + 1, sizeof(char));
						strcpy(labels[l], parsedStrings[i][0]);
						labelsData[l] = placeCounter;
					}
					break;

				case 1: // оператор
					if (!strcmp(parsedStrings[i][1], "start"))
						placeCounter = atoi(parsedStrings[i][2]); // конвертируем первый операнд в integer
					break;
				}
				/*
				case 3: // операнд 2
					t = 3;
					// без break, потому что действия аналогичны первому операнду

				case 2: // операнд 1
					// если это не число и не регистр, то заносим строку в таблицу имён, проверив сначала, не метка ли это
					if (! (parsedStrings[i][t][0] >= 48 && parsedStrings[i][t][0] <= 57 || // не число от 0 до 9
						   isKeyOf(registersTable, parsedStrings[i][t], r_m, r_s)))	 	   // и не регистр?
					{
						if (! isKeyOf(labelsTable, parsedStrings[i][t], l_m, l_s)) // не метка?
						{
							printf("2.1) %d  %s\n", strlen(parsedStrings[i][t]), parsedStrings[i][t]);
							labels[++l] = (char*)calloc(strlen(parsedStrings[i][k]), sizeof(char));
							strcpy(labels[l], parsedStrings[i][0]);
							Result *result = hashTable(labels, l + 1, -99999, 0); // (char (*)[STRING])
							labelsTable = result->Table;
							l_m = result->Data[0];
							l_s = result->Data[1];
						}
						else
						{	
							int data = getKey(labelsTable, parsedStrings[i][t], l_m, l_s)->Data;
							if (data > 0)
								modifyKey(labelsTable, parsedStrings[i][t], data, l_m, l_s);
						}
					}
				*/
			}
		}
		placeCounter++;
	}

	Result *result = hashTable(labels, l + 1, labelsData, 0); // (char (*)[STRING])
	return result;
}

int main(int argc, char *argv[])
{	
	printf("Initialization...\n\n");

	char c[NUMBER_OF_COMMANDS][STRING] = COMMANDS;
	char** commands = (char**) calloc(NUMBER_OF_COMMANDS, sizeof(char*));
	int *commandsData = (int*) calloc(NUMBER_OF_COMMANDS, sizeof(int)); int start = 13;
	for (int i = 0; i < NUMBER_OF_COMMANDS; i++)
	{
		commands[i] = (char*)calloc(STRING, sizeof(char));
		commandsData[i] = start++;
		strcpy(commands[i], c[i]);
	}

	char d[NUMBER_OF_DIRECTIVES][STRING] = DIRECTIVES;
	char** directives = (char**) calloc(NUMBER_OF_DIRECTIVES, sizeof(char*));
	int *directivesData = (int*) calloc(NUMBER_OF_DIRECTIVES, sizeof(int)); start = 43;
	for (int i = 0; i < NUMBER_OF_DIRECTIVES; i++)
	{
		directives[i] = (char*)calloc(STRING, sizeof(char));
		directivesData[i] = start++;
		strcpy(directives[i], d[i]);
	}

	char r[NUMBER_OF_REGISTERS][STRING] = REGISTERS;
	char** registers = (char**) calloc(NUMBER_OF_REGISTERS, sizeof(char*));
	int *registersData = (int*) calloc(NUMBER_OF_REGISTERS, sizeof(int)); start = 1;
	for (int i = 0; i < NUMBER_OF_REGISTERS; i++)
	{
		registers[i] = (char*)calloc(STRING, sizeof(char));
		registersData[i] = start++;
		strcpy(registers[i], r[i]);
	}
	
	printf("Хеш таблица команд:\n");
	Result *result = hashTable(commands, NUMBER_OF_COMMANDS, commandsData, 1);
	DataRecord *commandsTable = result->Table;
	int com_m = result->Data[0];
	int com_shift = result->Data[1];

	printf("\nХеш таблица директив:\n");
	result = hashTable(directives, NUMBER_OF_DIRECTIVES, directivesData, 1);
	DataRecord *directivesTable = result->Table;
	int dir_m = result->Data[0];
	int dir_shift = result->Data[1];

	printf("\nХеш таблица регистров:\n");
	result = hashTable(registers, NUMBER_OF_REGISTERS, registersData, 1);
	DataRecord *registersTable = result->Table;
	int reg_m = result->Data[0];
	int reg_shift = result->Data[1];

	char** t = (char**) calloc(1, sizeof(char*));
	t[0] = (char*)calloc(STRING, sizeof(char));
	int *tD = (int*) calloc(1, sizeof(int));
	tD[0] = 0;
	result = hashTable(t, 1, tD, 0);
	DataRecord *labelsTable = result->Table;
	int lab_m = result->Data[0];
	int lab_shift = result->Data[1];


	printf("\n\nWorking...\n\n");

	/* Parsing assembly strings */
	FILE *fp = fopen(argv[1], "r");
	char str[MAXCHAR];
	int numberOfStrings = 0;
	static char** parsedStrings[6];
	//parsedStrings = (char***) calloc(1, sizeof(char**));
	
	//int n = -1;
	while (fgets(str, MAXCHAR, fp) != NULL)
	{
		char** res = (char**) calloc(6, sizeof(char*));
		for (int i = 0; i < 5; i++)
		{	
			res[i] = (char*) calloc(strlen(str), sizeof(char));
		}

		//Parsing assembly string
		if (parseString(str, res))
		{
			++numberOfStrings;
			//printf("Оригинал: '%s'\n", rmSymbs(str, "\n"));
			parsedStrings[numberOfStrings - 1] = (char**) calloc(6, sizeof(char*));
			parsedStrings[numberOfStrings - 1] = res;
			//printParsedString(parsedStrings[numberOfStrings - 1]);
		}
	}
	fclose(fp);
	
	//printHashTable(commandsTable, com_m);
	//printHashTable(registersTable, reg_m);
	printf("Первый проход... \n\n");

	result = firstPass(parsedStrings, numberOfStrings, registersTable, reg_m, reg_shift, labelsTable, lab_m, lab_shift);
	labelsTable = result->Table;
	lab_m = result->Data[0];
	lab_shift = result->Data[1];

	puts("");
	//printf("%d  %d  %d  %d\n", lab_m, lab_shift);
	printHashTable(labelsTable, lab_m);
	puts("");
	//printHashTable(namesTable, nam_m);

	return 0;
}
