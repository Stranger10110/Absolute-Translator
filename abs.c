#include "parser.h"
#include "hash.h"
#include "abs.h"

int* firstPass(char** parsedStrings[6], int numberOfStrings, DataRecord *registersTable, int r_m, int r_s, DataRecord *labelsTable, int l_m, int l_s)
{	
	int placeCounter = 0;

	int l = -1;
	char** labels = (char**) calloc(numberOfStrings, sizeof(char*));
	/*for (int i = 0; i < numberOfStrings; i++)
		labels[i] = (char*) calloc(MAXCHAR, sizeof(char));*/

	//int n = -1;
	//char* names[STRING];// = (char**)calloc(1, sizeof(char*));
	/*for (int i = 0; i < numberOfStrings; i++)
		names[i] = (char*) calloc(MAXCHAR, sizeof(char));*/

	for (int i = 0; i < numberOfStrings; i++)
	{
		int t = 2; // по умолчанию только один операнд
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
						free(labels[0]);
						labels[0] = (char*) calloc(strlen(parsedStrings[i][0]), sizeof(char));
						strcpy(labels[0], parsedStrings[i][0]);
						
						//printf("%s\n", labels[l]);
						Result *result = reHashTable(labelsTable, l_m, labels, 1, -99999, 0); // (char (*)[STRING])
						//free(labelsTable);

						labelsTable = result->Table;
						l_m = result->Data[0];
						l_s = result->Data[1];
						//printHashTable(result->Table, l_m);
					}
					printHashTable(labelsTable, l_m);
					modifyKey(labelsTable, parsedStrings[i][0], placeCounter, l_m, l_s);
					printHashTable(labelsTable, l_m);
					break;

				case 1: // оператор
					if (!strcmp(parsedStrings[i][1], "start"))
						placeCounter = atoi(parsedStrings[i][2]); // конвертируем первый операнд в integer
					break;

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
		}
		placeCounter++;
	}

	static int result[2];
	result[0] = l_m; result[1] = l_s;
	return result;
}

int main(int argc, char *argv[])
{	
	printf("Initialization...\n\n");

	char c[NUMBER_OF_COMMANDS][STRING] = COMMANDS;
	char** commands = (char**)calloc(NUMBER_OF_COMMANDS, sizeof(char*));
	for (int i = 0; i < NUMBER_OF_COMMANDS; i++)
	{
		commands[i] = (char*)calloc(STRING, sizeof(char));
		strcpy(commands[i], c[i]);
	}

	char r[NUMBER_OF_REGISTERS][STRING] = REGISTERS;
	char** registers = (char**)calloc(NUMBER_OF_REGISTERS, sizeof(char*));
	for (int i = 0; i < NUMBER_OF_REGISTERS; i++)
	{
		registers[i] = (char*)calloc(STRING, sizeof(char));
		strcpy(registers[i], c[i]);
	}
	
	printf("Хеш таблица команд:\n");
	Result *result = hashTable(commands, NUMBER_OF_COMMANDS, 13, 1);
	DataRecord *commandsTable = result->Table;
	int com_m = result->Data[0];
	int com_shift = result->Data[1];

	printf("\nХеш таблица регистров:\n");
	result = hashTable(registers, NUMBER_OF_REGISTERS, 1, 1);
	DataRecord *registersTable = result->Table;
	int reg_m = result->Data[0];
	int reg_shift = result->Data[1];

	char** t = (char**) calloc(1, sizeof(char*));
	t[0] = (char*)calloc(STRING, sizeof(char));
	Result *res1 = hashTable(t, 1, -99999, 0);
	DataRecord *labelsTable = res1->Table;
	int lab_m = res1->Data[0];
	int lab_shift = res1->Data[1];


	printf("\n\nWorking...\n\n");

	/* Parsing assembly strings */
	FILE *fp = fopen(argv[1], "r");
	char str[MAXCHAR];
	int numberOfStrings = 0;
	char** parsedStrings[6];
	
	//int n = -1;
	while (fgets(str, MAXCHAR, fp) != NULL)
	{
		char** res = (char**) calloc(6, sizeof(char*));
		for (int i = 0; i < 5; i++)
		{	
			res[i] = (char*) calloc(strlen(str), sizeof(char));
		}

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
				
	//printHashTable(registersTable, reg_m);
	printf("Первый проход... \n\n");

	int *res = firstPass(parsedStrings, numberOfStrings, registersTable, reg_m, reg_shift, labelsTable, lab_m, lab_shift);
	lab_m = res[0];
	lab_shift = res[1];

	puts("");
	//printf("%d  %d  %d  %d\n", lab_m, lab_shift);
	printHashTable(labelsTable, lab_m);
	puts("");
	//printHashTable(namesTable, nam_m);

	return 0;
}
