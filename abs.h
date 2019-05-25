#include <ctype.h>

// codes: 1 - 33
#define COMMANDS {"inc", "dec", "add", "adc", "sub", "sbc", "mul", "imul", "div", "idiv", "cdw", "cwd", "cmp", "neg", "and", "or", "xor", "not", "test", "shl", "shr", "rol", "ror", "loop", "jmp", "jeq", "jne", "jg", "jge", "jl", "jle", "mov", "int"}
#define NUMBER_OF_COMMANDS 33

// codes: 1 - 12
#define REGISTERS {"ax", "ah", "al", "bx", "bh", "bl", "cx", "ch", "cl", "dx", "dh", "dl"}
#define NUMBER_OF_REGISTERS 12


#define COMMAND_SIZE 6 // признак адрессации - 4 старших бита, команда - следующие 12 бит, первый и второй операнды - по 2 байта
/*
	Признаки адрессации:
	0)  0000 - без операндов
	1)  0001 - регистр, адрес
	2)  0010 - адрес, регистр
	3)  0011 - регистр, число
	4)  0100 - число, регистр
	5)  0101 - адрес, число
	6)  0110 - число, адрес
	7)  0111 - регистр, регистр
	8)  1000 - адрес, адрес
	9)  1001 - регистр
	10) 1010 - адрес
	11) 1011 - число
*/
#define COM_PART_SIZE 2
//#define PLACE_COUNTER_MIN 13
#define RAM	2 //байта


/*
	Объектный файл:
	 1 байт - это признак записи (1 - заголовок, 2 - текст программы, 3 - окончание)
	 - Запись заголовка: 1 байт - признак записи, 2 байта - адресс начала размещения, 4 байта - длина программы, ? байт - имя файла
	 - Запись текста программы: 2 байта - адресс в памяти, 1 байт - длина записи, ? байт - значение
	 - Запись окончания: 1 байт - признак записи, 2 байта - адресс начала выполнения программы
*/

long pow_(int a, int n);

int fromHexToInt(char *hex);

Result* firstPass(char** parsedStrings[6], int numberOfStrings,
				  DataRecord *regTab, int r_m, int r_s, DataRecord *comTab, int c_m, int c_s,
				  DataRecord *labelsTable, int l_m, int l_s, int *counter, int *errorLine, int *codeSize);

int secondPass(char** parsedStrings[6], int numberOfStrings,
			   DataRecord *regTab, int r_m, int r_s, DataRecord *comTab, int c_m, int c_s,
			   DataRecord *labTab, int l_m, int l_s, int counter, int codeSize, char *filename);

