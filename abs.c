#include "parser.h"
#include "hash.h"
#include "abs.h"


Result* firstPass(char** parsedStrings[6], int numberOfStrings, DataRecord *regTab, int r_m, int r_s,
				  DataRecord *comTab, int c_m, int c_s, DataRecord *labelsTable, int l_m, int l_s, int *counter, int *errorLine)
{	
	int placeCounter = PLACE_COUNTER_MIN;
	*counter = placeCounter;

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
					*counter = placeCounter;
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
				else if (!isKeyOf(comTab, parsedStrings[i][1], c_m, c_s) && strcmp(parsedStrings[i][1], "end"))
				{	
					puts(parsedStrings[i][1]);
					Result* res;
					*errorLine = i + 1;
					return res;
				}
				break;
			
			case 3: // операнд 2
				t = 3;
				// без break, потому что действия аналогичны первому операнду

			case 2: // операнд 1
				/* 
					Если это не регистр, то заносим строку в таблицу меток, если её нет в таблице,
					иначе заменяем её на адрес. Если это регистр, заменяем его на код.
				*/
				if (!isKeyOf(regTab, parsedStrings[i][t], r_m, r_s)) // не регистр?
				{
					if (isKeyOf(labelsTable, parsedStrings[i][t], l_m, l_s)) // в таблице меток?
					{
						char str[STRING];
						sprintf(str, "%d", getKey(labelsTable, parsedStrings[i][t], l_m, l_s));
						strcpy(parsedStrings[i][t], str);
					}						
				}
				else// if (isKeyOf(regTab, parsedStrings[i][t], r_m, r_s))
				{
					char str[STRING];
					sprintf(str, "%d", getKey(regTab, parsedStrings[i][t], r_m, r_s));
					strcpy(parsedStrings[i][t], str);
				}
			}
		}
	}

	result = hashTable(labels, l + 1, labelsData, 0);
	return result;
}


int secondPass(char** parsedStrings[6], int numberOfStrings,
				DataRecord *regTab, int r_m, int r_s, DataRecord *comTab, int c_m, int c_s,
				DataRecord *labTab, int l_m, int l_s, int counter)
{	
	int placeCounter = counter;

	for (int i = 0; i < numberOfStrings; i++)
	{	
		/* Print address and object code */
		int t = 2, place = 0;
		for (int k = 1; k < 4; k++)
		{	
			if (strlen(parsedStrings[i][k]) != 0)
			{
				switch (k)
				{
				case 1: // оператор
					if (!strcmp(parsedStrings[i][1], "start") || !strcmp(parsedStrings[i][1], "end"))
						continue;
					
					//printf("%d) %d: ", i + 1, placeCounter);
					int spaces = 1;
					if (i + 1 < 10 && numberOfStrings > 9) spaces = 2;
					if (i + 1 < 100 && numberOfStrings > 99) spaces = 3;
					if (i + 1 < 10000 && numberOfStrings > 999) spaces = 4;
					printf("%d)%*c%0.*X: ", i + 1, spaces, ' ', RAM * 2, placeCounter);

					if (!strcmp(parsedStrings[i][1], "resb"))
					{	
						printf("%X\n", 0);
						placeCounter += atoi(parsedStrings[i][2]);
					}
					else if (!strcmp(parsedStrings[i][1], "resw"))
					{	
						printf("%0*X\n", atoi(parsedStrings[i][2]) * W * 2, 0);
						placeCounter += atoi(parsedStrings[i][2]) * W;
					}
					else if (!strcmp(parsedStrings[i][1], "byte"))
					{	
						for (int g = 0; g < strlen(parsedStrings[i][2]); g++)
							if (parsedStrings[i][2][g] != "'"[0] &&
								parsedStrings[i][2][g] != '"')
								printf("%c\n", parsedStrings[i][2][g]);
						printf("\n");
						placeCounter += strlen(parsedStrings[i][2]);
					}
					else if (!strcmp(parsedStrings[i][1], "word"))
					{	
						printf("%0.8X\n", atoi(parsedStrings[i][2]));
						placeCounter += W;
					}
					else
					{
						printf("%.2X", atoi(parsedStrings[i][1]));
						if (strlen(parsedStrings[i][3]) != 0)
							placeCounter += COMMAND_SIZE;
						else
							placeCounter += COMMAND_SIZE - 2;
						
						place = 1;
					}
					break;
				
				case 3: // первый операнд
					t = 3;
				
				case 2: // второй операнд
					if (isKeyOf(labTab, parsedStrings[i][t], l_m, l_s)) // в таблице меток?
					{
						//char str[STRING];
						//sprintf(str, "%d", );
						if (place)
							printf("%0.4X", getKey(labTab, parsedStrings[i][t], l_m, l_s));
						//strcpy(parsedStrings[i][t], str);
					}
					else if (parsedStrings[i][t][0] >= 48 && parsedStrings[i][t][0] <= 57)
					{	
						if (place)
							printf("%0.4X", atoi(parsedStrings[i][t]));
					}
					else
					{
						return i + 1; // error in string number...
					}
				}
			}
			/*else
			{	
				if (k == 2 || k == 3)
					printf("%0*X", 8, 0);
			}*/
			
		}

		if (place) puts("");
	}

	return 0;
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

	/*char d[NUMBER_OF_DIRECTIVES][STRING] = DIRECTIVES;
	char** directives = (char**) calloc(NUMBER_OF_DIRECTIVES, sizeof(char*));
	int *directivesData = (int*) calloc(NUMBER_OF_DIRECTIVES, sizeof(int)); start = 43;
	for (int i = 0; i < NUMBER_OF_DIRECTIVES; i++)
	{
		directives[i] = (char*)calloc(STRING, sizeof(char));
		directivesData[i] = start++;
		strcpy(directives[i], d[i]);
	}*/

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

	/*printf("\nХеш таблица директив:\n");
	result = hashTable(directives, NUMBER_OF_DIRECTIVES, directivesData, 1);
	DataRecord *directivesTable = result->Table;
	int dir_m = result->Data[0];
	int dir_shift = result->Data[1];*/

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
	//char*** originalStrings = (char***) calloc(1, sizeof(char**));
	
	//int n = 0;
	printf("Оригинал: \n\n");
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
			printf("%d) '%s'\n", numberOfStrings, rmSymbs(str, "\n"));
			parsedStrings[numberOfStrings - 1] = (char**) calloc(6, sizeof(char*));
			parsedStrings[numberOfStrings - 1] = res;

			/*originalStrings[numberOfStrings - 1] = (char**) calloc(6, sizeof(char*));
			for (int n = 0; n < 5; n++)
			{
				originalStrings[numberOfStrings-1][n] = (char*) calloc(strlen(res[n]), sizeof(char));
				strcpy(originalStrings[numberOfStrings-1][n], res[n]);
			}*/
			
			//printParsedString(parsedStrings[numberOfStrings - 1]);
		}
	}
	fclose(fp);
	puts("");
	
	//printHashTable(commandsTable, com_m, "команды");
	//printHashTable(registersTable, reg_m);
	//printf("Первый проход... \n\n");

	int placeCounterStart, errorLine = -1;
	result = firstPass(parsedStrings, numberOfStrings, registersTable, reg_m, reg_shift,
					   commandsTable, com_m, com_shift, labelsTable, lab_m, lab_shift, &placeCounterStart, &errorLine);
	if (errorLine == -1)
	{	
		labelsTable = result->Table;
		lab_m = result->Data[0];
		lab_shift = result->Data[1];
	}
	else
	{
		printf("Error in string #%d - unknown operator!\n", errorLine);
		return 0;
	}
	

	//puts("");
	//printf("%d  %d  %d  %d\n", lab_m, lab_shift);
	puts("Результат:\n");
	//printHashTable(namesTable, nam_m);

	/*for (int i = 0; i < numberOfStrings; i++)
	{
		printParsedString(parsedStrings[i]);
	}*/

	int res_code = secondPass(parsedStrings, numberOfStrings,
			   	   	   registersTable, reg_m, reg_shift, commandsTable, com_m, com_shift,
				   	   labelsTable, lab_m, lab_shift, placeCounterStart);
	if (res_code)
		printf("Error in string #%d - unknown identifier!\n", res_code);
	/*else
	{	
		puts("\n");
		printHashTable(labelsTable, lab_m, "метки");
	}*/

	return 0;
}
