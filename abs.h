// codes: 13 - 43
#define COMMANDS {"add", "adc", "sub", "sbc", "mul", "imul", "div", "idiv", "cdw", "cwd", "cmp", "neg", "and", "or", "xor", "not", "test", "shl", "shr", "rol", "ror", "loop", "jmp", "jeq", "jne", "jg", "jge", "jl", "jle", "mov", "int"}
#define NUMBER_OF_COMMANDS 31

// codes: 44 - 49
#define DIRECTIVES {"start", "end", "byte", "word", "resb", "resw"}
#define NUMBER_OF_DIRECTIVES 6

// codes: 1 - 12
#define REGISTERS {"ax", "ah", "al", "bx", "bh", "bl", "cx", "ch", "cl", "dx", "dh", "dl"}
#define NUMBER_OF_REGISTERS 12

//#define BIT_GRID 32
#define COMMAND_SIZE 5 // команда - 1 байт, первый и второй операнды - по 2 байта
#define PLACE_COUNTER_MIN 13

