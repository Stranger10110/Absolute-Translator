#include "parser.h"

char* rmSymbs(char *string, char *x)
{
	if (strlen(string) == 1) return string;

	int j = 0;
	char* new_string = (char*) calloc(strlen(string), sizeof(char));
	for (int i = 0; string[i] != '\0'; i++)
	{
		int include = 1;
		for (int k = 0; k < strlen(x); k++)
			if (string[i] == x[k]) include = 0;

		if (include) new_string[j++] = string[i];
	}
	new_string[j] = '\0';
	return new_string;
}

void toLowerCase(char *string)
{
	for (int i = 0; i < strlen(string); i++)
		if (string[i] < 97)
			string[i] += ' ';
}

int parseString(char *string, char* res[])
{
	if (!string) return 0;

	// Check length
	int length = strlen(string);
	switch (length)
	{
		case 1:
			return 0;
		case 2:
			return 0;
	}

	int hasComment = 0;
	if (length > COMMENT)
	{
		length = COMMENT;
		hasComment = 1;
	}

	// Parsing label, operator and operand
	char temp[MAXCHAR];
	int c = 0;
	int spaces = 0;
	for (int i = 0; i < length; i++)
	{
		switch (string[i])
		{
		case '\'': case '"':
		case ' ': case '	':
		case ',': case '\0':
			break;

		case LABEL:
			if (strlen(res[1]) != 0)
			{
				strcpy(res[0], res[1]);

				// очищаем оператор
				free(res[1]);
				res[1] = (char*) malloc(length);
			}
			else
			{
				strncpy(res[0], temp, c); // метка
				c = 0;
			}
			break;
		
		case COMMENT:
			c = 0;
			if (i + 1 != length - 1)
				for (int k = i + 1; k != length - 1; k++)
					temp[c++] = string[k];
			strncpy(res[4], temp, c); // комментарий
			return 1;

		default:
			temp[c++] = string[i]; // записываем каждый "значащий" символ во временный буфер

			if (((string[i+1] == ' ') || (string[i + 1] == '	') || (string[i + 1] == ',') ||
				(i + 1 == length - 1) || (string[i + 1] == COMMENT)|| (string[i + 1] == '\'')||
				(string[i + 1] == '"')))
			{	
				if (strlen(res[1]) == 0)
				{
					strncpy(res[1], temp, c); // оператор
					toLowerCase(res[1]);
					c = 0;
				}
				else
				{
					switch (spaces)
					{	
						case 0:
							strncpy(res[2], temp, c); // операнд 1
							res[2] = rmSymbs(res[2], ",");
							c = 0;
							break;

						case 1:
							strncpy(res[3], temp, c); // операнд 2
							res[3] = rmSymbs(res[3], ",");
							c = 0;
							break;
					}
					spaces++;
				}
			}
			if (string[i+1] == ',') ++i;
		}
	}

	c = 0;
	for (int i = 0; i < 5; i++)
		if (strlen(res[i]) == 0)
			++c;

	if (c != 5)
		return 1;
	else
		return 0;
}

void printParsedString(char* res[])
{
	int i = 0;
	printf("%s'%s' | ", "Метка: ", res[i++]);
	printf("%s'%s' | ", "Оператор: ", res[i++]);
	printf("%s'%s' | ", "Операнд_1: ", res[i++]);
	printf("%s'%s' | ", "Операнд_2: ", res[i++]);
	printf("%s'%s'\n\n\n", "Комментарий: ", res[i]);
}

/*int main_3(int argc, char *argv[])
{
	FILE *fp;
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
		char* res[6];
		for (int i = 0; i < 5; i++)
			res[i] = (char*) calloc(strlen(str), sizeof(char));

		if (parseString(str, res))
		{
			printf("Оригинал: '%s'\n", rmSymbs(str, "\n"));
			printParsedString(res);
		}
	}
	fclose(fp);
	return 0;
}*/