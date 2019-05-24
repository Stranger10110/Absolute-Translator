#include "parser.h"
#include "hash.h"
#include "abs.h"


long pow_(int a, int n)
{
	long res = 1;
	while (n-- > 0) res *= a;
	return res;
}


int fromHexToInt(char *hex)
{
	int result = 0;
	for (int i = 0; i < strlen(hex); i++)
	{
		switch (hex[i])
		{
		case 'a': case 'A':
			result += 10 * pow_(16, strlen(hex) - i - 1);
			break;
		
		case 'b': case 'B':
			result += 11 * pow_(16, strlen(hex) - i - 1); break;
		
		case 'c': case 'C':
			result += 12 * pow_(16, strlen(hex) - i - 1); break;
		
		case 'd': case 'D':
			result += 13 * pow_(16, strlen(hex) - i - 1); break;
		
		case 'e': case 'E':
			result += 14 * pow_(16, strlen(hex) - i - 1); break;
		
		case 'f': case 'F':
			result += 15 * pow_(16, strlen(hex) - i - 1); break;
		
		default:
			result += ((int)hex[i] - 48) * pow_(16, strlen(hex) - i - 1); break;
		}
	}

	return result;
}


int fromStrToInt(char *string)
{
	if (string[strlen(string) - 1] != 'h')
		return atoi(string);
	else
		return fromHexToInt(rmSymbs(string, "h"));
}


int numOfBytes(int x)
{
	int n = 8;
	while (x >= pow_(2, n)) n += 8;
	return n / 8;
}


int isNum(char *str)
{
	if (str[strlen(str) - 1] == 'h')
		return 1;
	else
	{
		for (int i = 0; i < strlen(str); i++)
			if (!isdigit(str[i])) return 0;
		return 1;
	}
	return 0;
}


Result* firstPass(char** parsedStrings[6], int numberOfStrings,
				  DataRecord *regTab, int r_m, int r_s, DataRecord *comTab, int c_m, int c_s,
				  DataRecord *labelsTable, int l_m, int l_s, int *counter, int *errorLine, int *codeSize)
{
	int placeCounter = 0;
	*counter = placeCounter;

	int l = -1;
	char** labels = (char**) calloc(numberOfStrings, sizeof(char*));
	int *labelsData = (int*) calloc(numberOfStrings, sizeof(int));
	Result *result;

	for (int i = 0; i < numberOfStrings; i++)
	{	
		*codeSize = placeCounter - *counter; // размер кода

		/* Проверяем, какая адрессация */
		int signOfAdress = 0; // по умолчанию без операндов
		if (strlen(parsedStrings[i][3]) == 0)
		{
			if (isKeyOf(regTab, parsedStrings[i][2], r_m, r_s))
				signOfAdress = 9 << 12;
			else if (isNum(parsedStrings[i][2]))
				signOfAdress = 11 << 12;
			else
				signOfAdress = 10 << 12;
		}
		else
		{
			if (isKeyOf(regTab, parsedStrings[i][2], r_m, r_s) && isNum(parsedStrings[i][3]))
				signOfAdress = 3 << 12;
			else if (isNum(parsedStrings[i][2]) && isKeyOf(regTab, parsedStrings[i][3], r_m, r_s))
				signOfAdress = 4 << 12;
			else if (isKeyOf(regTab, parsedStrings[i][2], r_m, r_s) && isKeyOf(regTab, parsedStrings[i][3], r_m, r_s))
				signOfAdress = 7 << 12;
			else if (isKeyOf(regTab, parsedStrings[i][2], r_m, r_s))
				signOfAdress = 1 << 12;
			else if (isKeyOf(regTab, parsedStrings[i][3], r_m, r_s))
				signOfAdress = 2 << 12;
			else if (isNum(parsedStrings[i][3]))
				signOfAdress = 5 << 12;
			else if (isNum(parsedStrings[i][2]))
				signOfAdress = 6 << 12;
			else
				signOfAdress = 8 << 12;
		}
		

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
				if (labelsTable == NULL || !isKeyOf(labelsTable, parsedStrings[i][0], l_m, l_s))
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
					placeCounter = fromStrToInt(parsedStrings[i][2]);
					*counter = placeCounter;
				}
				else if (!strcmp(parsedStrings[i][1], "resb"))
				{
					placeCounter += fromStrToInt(parsedStrings[i][2]);
				}
				else if (!strcmp(parsedStrings[i][1], "resw"))
				{
					placeCounter += fromStrToInt(parsedStrings[i][2]) * W;
				}
				else if (!strcmp(parsedStrings[i][1], "byte"))
				{	
					int n = fromStrToInt(parsedStrings[i][2]);
					if (isNum(parsedStrings[i][2]))
						placeCounter += numOfBytes(n);
					else
						placeCounter += strlen(parsedStrings[i][2]);
				}
				else if (!strcmp(parsedStrings[i][1], "word"))
				{
					placeCounter += W;
				}
				else if (isKeyOf(comTab, parsedStrings[i][1], c_m, c_s))
				{
					if (strlen(parsedStrings[i][3]) != 0)
						placeCounter += COMMAND_SIZE;
					else if (strlen(parsedStrings[i][2]) != 0)
						placeCounter += COMMAND_SIZE - 2;
					else
						placeCounter += COMMAND_SIZE - 4;

					char str[STRING];
					sprintf(str, "%d", getKey(comTab, parsedStrings[i][1], c_m, c_s) + signOfAdress);
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
					if (labelsTable != NULL && isKeyOf(labelsTable, parsedStrings[i][t], l_m, l_s)) // в таблице меток?
					{
						char str[STRING];
						sprintf(str, "%d", getKey(labelsTable, parsedStrings[i][t], l_m, l_s));
						strcpy(parsedStrings[i][t], str);
					}
				}
				else
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
				DataRecord *labTab, int l_m, int l_s, int counter, int codeSize, char *filename)
{	
	int sign = 1, buffer, placeCounter = counter;
	FILE *object_file = fopen("result.obj", "wb");

	for (int i = 0; i < numberOfStrings; i++)
	{	
		/* Print address and object code */
		int t = 2, place = 0, spaces = 1, byte_command = 0;
		for (int k = 1; k < 4; k++)
		{	
			if (strlen(parsedStrings[i][k]) != 0)
			{
				switch (k)
				{
				case 1: // оператор
					if (!strcmp(parsedStrings[i][1], "start"))				 // START
					{	
						buffer = fromStrToInt(parsedStrings[i][2]);
						fwrite(&sign, sizeof(char), 1, object_file);		 // запись заголовка
						fwrite(&buffer, (size_t)2, 1, object_file);			 // операнд start
						buffer = codeSize;
						fwrite(&buffer, (size_t)4, 1, object_file); 	 	 // длина программы
						fwrite(filename, (size_t)strlen(filename), 1, object_file); // имя файла
						++sign; continue;
					}
					else if (!strcmp(parsedStrings[i][1], "end"))			 // END
					{
						++sign;
						fwrite(&sign, sizeof(char), 1, object_file);		 // запись окончания
						buffer = atoi(parsedStrings[i][2]);
						fwrite(&buffer, (size_t)2, 1, object_file);			 // операнд end
						continue;
					}
					

					if (i + 1 < 10 && numberOfStrings > 9) spaces = 2;
					if (i + 1 < 100 && numberOfStrings > 99) spaces = 3;
					if (i + 1 < 10000 && numberOfStrings > 999) spaces = 4;
					printf("%d)%*c%0.*X: ", i + 1, spaces, ' ', RAM * 2, placeCounter);
					if (!strcmp(parsedStrings[i][1], "resb"))				 // RESB 
					{	
						printf("%0*X\n", atoi(parsedStrings[i][2]) * 2, 0);

						fwrite(&sign, sizeof(char), 1, object_file);		 // запись тела программы
						buffer = atoi(parsedStrings[i][2]) + 2;
						fwrite(&buffer, (size_t)1, 1, object_file);			 // длина записи
						fwrite(&placeCounter, (size_t)2, 1, object_file);	 // адресс
						buffer = 0;
						fwrite(&buffer, (size_t)1, atoi(parsedStrings[i][2]), object_file); // нули
						
						placeCounter += atoi(parsedStrings[i][2]);
					}
					else if (!strcmp(parsedStrings[i][1], "resw"))			 // RESW 
					{	
						printf("%0*X\n", atoi(parsedStrings[i][2]) * W * 2, 0);

						fwrite(&sign, sizeof(char), 1, object_file);		 // запись тела программы
						buffer = atoi(parsedStrings[i][2]) * W + 2;
						fwrite(&buffer, (size_t)1, 1, object_file);			 // длина записи
						fwrite(&placeCounter, (size_t)2, 1, object_file);	 // адресс
						buffer = 0;
						fwrite(&buffer, (size_t)1, atoi(parsedStrings[i][2]) * W, object_file);	// нули

						placeCounter += atoi(parsedStrings[i][2]) * W;
					}
					else if (!strcmp(parsedStrings[i][1], "byte"))			 // BYTE 
					{	
						int n = -1;
						if (isNum(parsedStrings[i][2]))
						{	
							n = fromStrToInt(parsedStrings[i][2]);
							buffer = numOfBytes(n) + 2;
						}
						else
						{
							buffer = strlen(parsedStrings[i][2]) + 2;
						}

						fwrite(&sign, sizeof(char), 1, object_file);		 // запись тела программы
						fwrite(&buffer, (size_t)1, 1, object_file);			 // длина записи
						fwrite(&placeCounter, (size_t)2, 1, object_file);	 // адресс
						if (n == -1)
						{	
							char *str = rmSymbs(parsedStrings[i][2], "\"\'");
							for (int i = 0; i < strlen(str); i++)
							{
								printf("%0.2X", (int)str[i]);
								int k = (int)str[i];
								fwrite(&k, (size_t)1, 1, object_file);		 // символьная константа
							}
							puts("");
						}
						else
						{	
							fwrite(&n, (size_t)numOfBytes(n), 1, object_file);
							printf("%0*X\n", numOfBytes(n) * 2, n);
						}

						placeCounter += buffer - 2; byte_command = 1;
					}
					else if (!strcmp(parsedStrings[i][1], "word"))			 // WORD
					{	
						printf("%0.8X\n", atoi(parsedStrings[i][2]));

						fwrite(&sign, sizeof(char), 1, object_file);		 // запись тела программы
						buffer = 4;
						fwrite(&buffer, (size_t)1, 1, object_file);			 // длина записи
						fwrite(&placeCounter, (size_t)2, 1, object_file);	 // адресс
						buffer = atoi(parsedStrings[i][2]);
						fwrite(&buffer, (size_t)W, 1, object_file);	 		 // число размером W байта

						placeCounter += W;
					}
					else													 // COMMAND 
					{
						printf("%.2X ", atoi(parsedStrings[i][1]));

						fwrite(&sign, sizeof(char), 1, object_file);		 // запись тела программы
						if (strlen(parsedStrings[i][3]) != 0)
							buffer = COMMAND_SIZE + 2;
						else if (strlen(parsedStrings[i][2]) != 0)
							buffer = COMMAND_SIZE;
						else
							buffer = COMMAND_SIZE - 2;
						fwrite(&buffer, (size_t)1, 1, object_file);			 // длина записи
						fwrite(&placeCounter, (size_t)2, 1, object_file);	 // адресс
						buffer = atoi(parsedStrings[i][1]);
						fwrite(&buffer, (size_t)COM_PART_SIZE, 1, object_file);	// команда (1 байт)

						if (strlen(parsedStrings[i][3]) != 0)
							placeCounter += COMMAND_SIZE;
						else if (strlen(parsedStrings[i][2]) != 0)
							placeCounter += COMMAND_SIZE - 2;
						else
							placeCounter += COMMAND_SIZE - 4;
						place = 1;
					}
					break;
				
				case 3: // первый операнд
					t = 3;
				
				case 2: // второй операнд
					if (isKeyOf(labTab, parsedStrings[i][t], l_m, l_s)) // в таблице меток?
					{
						if (place)
						{
							printf("%0.4X ", getKey(labTab, parsedStrings[i][t], l_m, l_s));
							buffer = getKey(labTab, parsedStrings[i][t], l_m, l_s);
							fwrite(&buffer, (size_t)COM_PART_SIZE, 1, object_file);	// машинная команда (3 или 5 байт)
						}
					}
					else if (isNum(parsedStrings[i][t])) // число?
					{	
						if (place)
						{
							printf("%0.4X ", atoi(parsedStrings[i][t]));
							buffer = atoi(parsedStrings[i][t]);
							fwrite(&buffer, (size_t)COM_PART_SIZE, 1, object_file);	// машинная команда (3 или 5 байт)
						}
					}
					else if (!byte_command)
					{
						return i + 1; // error in string number...
					}
				}
			}
			
		}

		if (place) puts("");
	}

	return 0;
}


int main(int argc, char *argv[])
{	
	FILE *fp = fopen(argv[1], "r");
	if (!fp)
	{
		printf("Can't open '%s'!\n", argv[1]);
		return 0;
	}

	printf("Initialization...\n\n");

	char c[NUMBER_OF_COMMANDS][STRING] = COMMANDS;
	char** commands = (char**) calloc(NUMBER_OF_COMMANDS, sizeof(char*));
	int *commandsData = (int*) calloc(NUMBER_OF_COMMANDS, sizeof(int)); int start = 1;
	for (int i = 0; i < NUMBER_OF_COMMANDS; i++)
	{
		commands[i] = (char*)calloc(STRING, sizeof(char));
		commandsData[i] = start++;
		strcpy(commands[i], c[i]);
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

	printf("\nХеш таблица регистров:\n");
	result = hashTable(registers, NUMBER_OF_REGISTERS, registersData, 1);
	DataRecord *registersTable = result->Table;
	int reg_m = result->Data[0];
	int reg_shift = result->Data[1];

	int lab_m = 1, lab_shift = 1;
	DataRecord *labelsTable = NULL;


	printf("\n\nWorking...\n\n");

	/* Parsing assembly strings */
	char str[MAXCHAR];
	int numberOfStrings = 0;
	static char** parsedStrings[6];
	
	printf("Оригинал: \n\n");
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
			printf("%d) '%s'\n", numberOfStrings, rmSymbs(str, "\n"));
			parsedStrings[numberOfStrings - 1] = (char**) calloc(6, sizeof(char*));
			parsedStrings[numberOfStrings - 1] = res;
		}
	}
	fclose(fp);
	puts("");

	int placeCounterStart, errorLine = -1, codeSize;
	result = firstPass(parsedStrings, numberOfStrings, registersTable, reg_m, reg_shift,
					   commandsTable, com_m, com_shift, labelsTable, lab_m, lab_shift,
					   &placeCounterStart, &errorLine, &codeSize);
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
	

	puts("Результат:\n");
	int res_code = secondPass(parsedStrings, numberOfStrings, registersTable, reg_m, reg_shift,
							  commandsTable, com_m, com_shift, labelsTable, lab_m, lab_shift,
							  placeCounterStart, codeSize, argv[1]);
	if (res_code)
		printf("Error in string #%d - unknown identifier!\n", res_code);
	
	//printHashTable(commandsTable, com_m, "команды");

	return 0;
}
