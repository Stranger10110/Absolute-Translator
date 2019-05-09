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


int insertKey(DataRecord* hashTable, char *key, int h, int code)
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
			
			if (h == M - 1) h = -1;
			while (hashTable[++h].key)
				if (h == M - 1) h = -1;

			hashTable[h].key = (char*) calloc(STRING, sizeof(char));
			if (hashTable[h].key)
				strcpy(hashTable[h].key, key);
			hashTable[h].Data = code;
		}
	
	return collision;
}


int getKey(DataRecord *hashTable, char* key, int m)
{
	int h = Hash(key, m, 13);

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


void printHashTable(DataRecord* hashTable, int m)
{
	puts("Hash table:");
	for (size_t i = 0; i < m; i++)
	{	
		if (hashTable[i].key)
			printf("%d) Key: %s, Data: %d\n\n", i, hashTable[i].key, hashTable[i].Data);
		else
			printf("%d)\n\n", i);
	}
	puts(" ");
}


DataRecord* initHashTable(char words[][STRING], int num, int m, int shift, int *colls)
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

	int code = 1;
	int collisions = 0;
	for (int i = 0; i < num; i++)
	{
		int hash = Hash(words[i], m, 13);
		collisions += insertKey(hashTable, words[i], hash, code++);
	}

	*colls = collisions;
	return hashTable;
}

Result* hashTable(char words[][STRING])
{
	int collisions, m = 43;
	int simpleNumbers[989] = SIMPLE_NUMBERS, k = 0;

	do
	{
		int shift = 1;
		do
		{
			DataRecord* commandsTable = initHashTable(words, 32, simpleNumbers[k++], shift++, &collisions);
			if (collisions <= 2)
			{
				printf("M = %d, shift = %d, collisions = %d\n", simpleNumbers[k - 1], shift - 1, collisions);
				//printf("ComTab: %s: %d", commandsTable[3].key, commandsTable[3].Data);
				Result *res = (Result*) calloc(1, sizeof(Result));
				res->Table = (DataRecord*) calloc(1, sizeof(DataRecord));
				//printf("ComTab: %s: %d", commandsTable[3].key, commandsTable[3].Data);
				res->Table = commandsTable;
				//int e[1]; e[0] = simpleNumbers[k - 1];
				//printf("%d\n", e[0]);
			
				res->Data[0] = simpleNumbers[k - 1];
				return res;
			}
		}
		while (shift <= 31);
	}
	while (k <= 989);
}

int main_2()
{
	srand(time(NULL));

	/*char words[17][STRING] = {"END", "BYTE", "START", "RESB", "RESW", "WORD",
				  "add", "adc", "and", "sub", "sbc", "mul", "imul",
				  "neg", "div", "idiv", "or"};
	*/
	char words[32][STRING] = COMMANDS;

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
		collisions += insertKey(hashTable, words[i], hash, 1);
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
