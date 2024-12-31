#include <stdlib.h>
#include <stdio.h>

#define ASCII_BITS 7
#define ASCII_MASK 0x7f
#define COUNT_BITS 3
#define COUNT_MASK 0x07
#define COUNTS_MASK 9
#define CHARS_PER_INT 3


FILE * fopen_checked(const char * const name, const char * const mode); 
