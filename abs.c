#include "parser.h"
#include "hash.h"
#include "abs.h"


Result* firstPass(char** parsedStrings[6], int numberOfStrings, DataRecord *regTab, int r_m, int r_s,
				  DataRecord *comTab, int c_m, int c_s, DataRecord *labelsTable, int l_m, int l_s)
{	
	int placeCounter = PLACE_COUNTER_MIN;

	int l = -1;
	char** labels = (char**) calloc(numberOfStrings, sizeof(char*));
	int *labelsData = (int*) calloc(numberOfStrings, sizeof(int));
	Result *result;

	for (int i = 0; i < numberOfStrings; i++)
	{	
		int t = 2;
		for (int k = 0; k < 4; k++)
		{	
			if (strlen(parsedStrings[i][k]) != 0)
			{	
				switch (k)
				{
				case 0: // метка
					/*
						Если метки нет в таблице, заносим её туда и назначаем текущий адрес размещений
					*/
					if (!isKeyOf(labelsTable, parsedStrings[i][0], l_m, l_s))
					{	
						labels[++l] = (char*) calloc(strlen(parsedStrings[i][0]), sizeof(char));
						strcpy(labels[l], parsedStrings[i][0]);
						labelsData[l] = placeCounter;
						result = hashTable(labels, l + 1, labelsData, 0);
						labelsTable = result->Table;
						l_m = result->Data[0];
						l_s = result->Data[1];
					}
					break;

				case 1: // оператор
					if (!strcmp(parsedStrings[i][1], "start"))
					{
						placeCounter = atoi(parsedStrings[i][2]); // конвертируем первый операнд в integer
						if (placeCounter < PLACE_COUNTER_MIN)
							placeCounter = PLACE_COUNTER_MIN;
					}
					else if (!strcmp(parsedStrings[i][1], "resb"))
						placeCounter += atoi(parsedStrings[i][2]);
					else if (!strcmp(parsedStrings[i][1], "resw"))
						placeCounter += atoi(parsedStrings[i][2]) * W;
					else if (!strcmp(parsedStrings[i][1], "byte"))
						placeCounter += strlen(parsedStrings[i][2]);
					else if (!strcmp(parsedStrings[i][1], "word"))
						placeCounter += W;
					else if (isKeyOf(comTab, parsedStrings[i][1], c_m, c_s))
					{
						placeCounter += COMMAND_SIZE;

						char str[STRING];
						sprintf(str, "%d", getKey(comTab, parsedStrings[i][1], c_m, c_s));
						strcpy(parsedStrings[i][1], str);
					}
					break;
				
				case 3: // операнд 2
					t = 3;
					// без break, потому что действия аналогичны первому операнду

				case 2: // операнд 1
					/* 
						Если это не число и не регистр, то заносим строку в таблицу меток, если её нет в таблице,
						иначе заменяем её на адрес. Если это регистр, заменяем его на код.
					*/
					if (!((parsedStrings[i][t][0] >= 48 && parsedStrings[i][t][0] <= 57) || // не число от 0 до 9
						isKeyOf(regTab, parsedStrings[i][t], r_m, r_s)))	 	   		  // и не регистр?
					{
						if (isKeyOf(labelsTable, parsedStrings[i][t], l_m, l_s)) // в таблице меток?
						{
							char str[STRING];
							sprintf(str, "%d", getKey(labelsTable, parsedStrings[i][t], l_m, l_s));
							strcpy(parsedStrings[i][t], str);
						}						
					}
					else if (isKeyOf(regTab, parsedStrings[i][t], r_m, r_s))
					{
						char str[STRING];
						sprintf(str, "%d", getKey(regTab, parsedStrings[i][t], r_m, r_s));
						strcpy(parsedStrings[i][t], str);
					}
				}
			}
		}
	}

	result = hashTable(labels, l + 1, labelsData, 0);
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

	result = firstPass(parsedStrings, numberOfStrings, registersTable, reg_m, reg_shift,
					   commandsTable, com_m, com_shift, labelsTable, lab_m, lab_shift);
	labelsTable = result->Table;
	lab_m = result->Data[0];
	lab_shift = result->Data[1];

	puts("");
	//printf("%d  %d  %d  %d\n", lab_m, lab_shift);
	printHashTable(labelsTable, lab_m);
	puts("");
	//printHashTable(namesTable, nam_m);

	for (int i = 0; i < numberOfStrings; i++)
	{
		printParsedString(parsedStrings[i]);
	}

	return 0;
}
