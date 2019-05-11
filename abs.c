#include "parser.h"
#include "hash.h"
#include "abs.h"

int* firstPass(char** parsedStrings[6], int numberOfStrings, DataRecord *registersTable, int r_m, int r_s, DataRecord *labelsTable, int l_m, int l_s, DataRecord *namesTable, int n_m, int n_s)
{	
	int placeCounter = 1000;

	char** labels; int l = -1;
	for (int i = 0; i < numberOfStrings; i++)
		labels[i] = (char*) calloc(MAXCHAR, sizeof(char));

	char** names; int n = -1;
	for (int i = 0; i < numberOfStrings; i++)
		names[i] = (char*) calloc(MAXCHAR, sizeof(char));

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
					/* если метки нет в таблице, заносим её туда и назначаем текущий адресс размещений */
					if (! isKeyOf(labelsTable, parsedStrings[i][0], l_m, l_s))
					{
						strcpy(labels[++l], parsedStrings[i][0]);
						Result *result = hashTable((char (*)[STRING])labels, l + 1, 1, 0);
						DataRecord *labelsTable = result->Table;
						int l_m = result->Data[0];
						int l_s = result->Data[1];
					}				
					modifyKey(labelsTable, parsedStrings[i][0], placeCounter, l_m, l_s);
					break;

				case 1: // оператор
					if (!strcmp(parsedStrings[i][1], "start"))
						placeCounter = atoi(parsedStrings[i][2]); // конвертируем первый операнд в integer

				case 3: // операнд 2
					t = 3;
					// без break, потому что действия аналогичны первому операнду

				case 2: // операнд 1
					/* если это не число и не регистр, то заносим строку в таблицу имён, проверив сначала, не метка ли это */
					if (! (parsedStrings[i][t][0] >= 48 && parsedStrings[i][t][0] <= 57 && // не число от 0 до 9
						   isKeyOf(registersTable, parsedStrings[i][t], r_m, r_s)))		   // и не регистр?
					{
						if (! isKeyOf(labelsTable, parsedStrings[i][t], l_m, l_s)) // не метка?
						{
							if (! isKeyOf(namesTable, parsedStrings[i][t], n_m, n_s)) // нет в таблице имён?
							{
								strcpy(names[++n], parsedStrings[i][t]);
								Result *result = hashTable((char(*)[STRING])names, l + 1, 1, 0);
								DataRecord *labelsTable = result->Table;
								int n_m = result->Data[0];
								int n_s = result->Data[1];
							}
						}
					}
				}
			}
		}

		placeCounter++;
	}

	static int result[4]; //{ l_m, l_s, n_m, n_s };
	result[0] = l_m; result[1] = l_s;
	result[2] = n_m; result[3] = n_s;
	return result;
}

int main(int argc, char *argv[])
{	
	printf("Initialization...\n\n");

	printf("Хеш таблица команд:\n");
	char commands[NUMBER_OF_COMMANDS][STRING] = COMMANDS;
	Result *result = hashTable(commands, NUMBER_OF_COMMANDS, 13, 1);
	DataRecord *commandsTable = result->Table;
	int com_m = result->Data[0];
	int com_shift = result->Data[1];

	printf("\nХеш таблица регистров:\n");
	char registers[NUMBER_OF_REGISTERS][STRING] = REGISTERS;
	result = hashTable(registers, NUMBER_OF_REGISTERS, 1, 1);
	DataRecord *registersTable = result->Table;
	int reg_m = result->Data[0];
	int reg_shift = result->Data[1];

	char t[][STRING] = { "", "a" };
	result = hashTable(t, 2, 1, 0);
	DataRecord *labelsTable = result->Table;
	int lab_m = result->Data[0];
	int lab_shift = result->Data[1];

	result = hashTable(t, 2, 1, 0);
	DataRecord *namesTable = result->Table;
	int nam_m = result->Data[0];
	int nam_shift = result->Data[1];


	printf("\n\nWorking...\n\n");

	/* Parsing assembly strings */
	FILE *fp = fopen(argv[1], "r");
	char str[MAXCHAR];
	int numberOfStrings = 0;
	char** parsedStrings[6];
	
	//int n = -1;
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
			parsedStrings[numberOfStrings - 1] = (char**) calloc(6, sizeof(char*));
			parsedStrings[numberOfStrings - 1] = res;
		}
	}
	fclose(fp);

	printf("Первый проход... \n\n");

	int *res = firstPass(parsedStrings, numberOfStrings, registersTable, reg_m, reg_shift, labelsTable, lab_m, lab_shift, namesTable, nam_m, nam_shift);
	lab_m = res[0];
	lab_shift = res[1];
	nam_m = res[0];
	nam_shift = res[1];

	puts("");
	printHashTable(labelsTable, lab_m);
	puts("");
	printHashTable(namesTable, nam_m);

	return 0;
}
