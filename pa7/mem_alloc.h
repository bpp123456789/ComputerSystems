//Authors: William Petrik - petrikw@bc.edu, Kevin O'Neill - oneillhn@bc.edu

#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define FAILURE -1
#define PAGE_SIZE 4096
#define SUCCESS 0
#define WORD_SIZE 8

typedef struct Header {
    size_t size;
    struct Header * next;
    struct Header * previous;
} Header;

void mem_free(void * ptr);
void * mem_alloc(size_t requested_size);

#endif
