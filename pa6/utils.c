#include <stdio.h>
#include <stdlib.h>

FILE * fopen_checked(const char * const name, const char * const mode)  {
    FILE * fp = fopen(name, mode);
    if (!fp) {
        perror("Cannot open the file");
        exit(EXIT_FAILURE);
    }
    return fp;
}

