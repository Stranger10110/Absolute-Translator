#include "hash.h"
#include "abs.h"

int Hash(char *key, int m, int shift)
{	
	// сложение многобайтового ключа по модулю WORD
	int i = 0, t = 3;
	unsigned int new_key = 0;
	unsigned int temp = 0;
	while (i < strlen(key))
	{
		temp = *(key + i);
		temp <<= 8 * t--;
		if (i != strlen(key) - 1)
			for(size_t k = i + 1; k <= i + 3; k++)
			{	
				if (k < strlen(key))
					temp |= *(key + k) << (8 * t--);
				else
					break;
			}
		t = 3;

		// циклический сдвиг temp влево на shift бит
		temp = (temp << shift) | (temp >> (32 - shift));

		new_key = (new_key + temp) % WORD;
		i += 4;
	}

	// хеш функция
	return new_key % m;
}


int insertKey(DataRecord* hashTable, char *key, int h, int code, int m)
{
	int collision = 0;
	if (!hashTable[h].key)
		{
			hashTable[h].key = (char*) calloc(STRING, sizeof(char));
			if (hashTable[h].key)
				strcpy(hashTable[h].key, key);
			hashTable[h].Data = code;
		}
		else
		{	
			collision++;
			
			if (h == m - 1) h = -1;
			while (hashTable[++h].key)
				if (h == m - 1) h = -1;

			hashTable[h].key = (char*) calloc(STRING, sizeof(char));
			if (hashTable[h].key)
				strcpy(hashTable[h].key, key);
			hashTable[h].Data = code;
		}
	
	return collision;
}


int getKey(DataRecord *hashTable, char* key, int m, int shift)
{
	int h = Hash(key, m, shift);

	if (!hashTable[h].key)
	{
		printf("\nNo key '%s' in hash table!\n", key);
		return 0;
	}
	else if (!strcmp(hashTable[h].key, key))
		return hashTable[h].Data;
	else
	{
		if (h == m - 1) h = -1;
		while (++h < m)
		{
			if (!hashTable[h].key)
			{
				printf("\nNo key '%s' in hash table!\n", key);
				return 0;
			}
			else if (!strcmp(hashTable[h].key, key))
				return hashTable[h].Data;

			if (h == m - 1) h = -1;
		}
		return hashTable[h].Data;
	}
}

int modifyKey(DataRecord *hashTable, char* key, int data, int m, int shift)
{
	int h = Hash(key, m, shift);

	if (!hashTable[h].key)
	{
		return 0;
	}
	else if (!strcmp(hashTable[h].key, key))
	{
		hashTable[h].Data = data;
		return 1;
	}
	else
	{
		if (h == m - 1) h = -1;
		while (++h < m)
		{
			if (!hashTable[h].key)
			{
				return 0;
			}
			else if (!strcmp(hashTable[h].key, key))
			{
				hashTable[h].Data = data;
				return 1;
			}

			if (h == m - 1) h = -1;
		}

		hashTable[h].Data = data;
		return 1;
	}
}


void printHashTable(DataRecord* hashTable, int m)
{
	puts("Hash table:");
	for (int i = 0; i < m; i++)
	{	
		if (hashTable[i].key)
			printf("%d) Key: %s, Data: %d\n\n", i, hashTable[i].key, hashTable[i].Data);
		else
			printf("%d)\n\n", i);
	}
	puts(" ");
}


DataRecord* initHashTable(char words[][STRING], int num, int m, int shift, int data, int *colls)
{
	DataRecord *hashTable = (DataRecord*) calloc(m, sizeof(DataRecord));
	if (hashTable)
	{
		for(size_t i = 0; i < m; i++)
		{
			hashTable[i].key = NULL;
			hashTable[i].Data = 0;
		}
	}

	int collisions = 0;
	for (int i = 0; i < num; i++)
	{
		int hash = Hash(words[i], m, shift);
		collisions += insertKey(hashTable, words[i], hash, data++, m);
	}

	*colls = collisions;
	return hashTable;
}

Result* hashTable(char words[][STRING], int data)
{
	int collisions, m = 43;
	int simpleNumbers[988] = SIMPLE_NUMBERS, k = -1;

	for (int k = 0; k <= 989; k++)
	{
		for (int shift = 1; shift <= 31; shift++)
		{
			DataRecord* commandsTable = initHashTable(words, NUMBER_OF_COMMANDS, simpleNumbers[k], ++shift, data, &collisions);
			if (collisions <= 3)
			{
				printf("размер = %d, коллизии = %d\n", simpleNumbers[k], collisions);

				Result *res = (Result*)calloc(1, sizeof(Result));
				res->Table = (DataRecord*)calloc(1, sizeof(DataRecord));
				res->Table = commandsTable;
				res->Data[0] = simpleNumbers[k];
				res->Data[1] = shift;

				return res;
			}
		}
	}
}

int main_2()
{
	srand(time(NULL));

	/*char words[17][STRING] = {"END", "BYTE", "START", "RESB", "RESW", "WORD",
				  "add", "adc", "and", "sub", "sbc", "mul", "imul",
				  "neg", "div", "idiv", "or"};
	*/
	char words[NUMBER_OF_COMMANDS][STRING] = COMMANDS;

	DataRecord *hashTable = (DataRecord*) calloc(M, sizeof(DataRecord));
	if (hashTable)
	{
		for(size_t i = 0; i < M; i++)
		{
			hashTable[i].key = NULL;
			hashTable[i].Data = 0;
		}
	}


	int collisions = 0;
	for (int i = 0; i < N; i++)
	{
		int hash = Hash(words[i], M, 13);
		collisions += insertKey(hashTable, words[i], hash, 1, M);
	}

	printf("Кол-во коллизий: %d\n\n", collisions);
	//printHashTable(hashTable, M);

	/*printf("Get 'idiv': '%d'\n", getKey(hashTable, M, "idiv"));
	printf("Get 'sbc': '%d'\n", getKey(hashTable, M, "sbc"));
	printf("Get 'neg': '%d'\n", getKey(hashTable, M, "neg"));
	printf("Get 'dog': '%d'\n", getKey(hashTable, M, "dog"));
	printf("Get 'ice_cream': '%d'\n", getKey(hashTable, M, "ice_cream"));
	printf("Get 'a#d$hj!**03@9j$fm': '%d'\n", getKey(hashTable, M, "aa#d$hj!**03@9j$fm"));*/

	free(hashTable);
}
