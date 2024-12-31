//Authors: William Petrik - petrikw@bc.edu, Kevin O'Neill - oneillhn@bc.edu

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h> 
#include <sys/mman.h>

#include "mem_alloc.h" 

Header * free_list = NULL; 

int is_allocated(Header * header) { 
    return header->size & 1; 
}

int is_free(Header * header) {
    return !(header->size & 1); 
}

void set_allocated(Header * header) {
    header->size |= 1; 
}

void set_free(Header * header) {
    header->size >>=1;
    header->size <<=1; 
}

Header * get_header(void * mem) {
    return (Header *) ((char *) mem - sizeof(Header)); 
}

int same_page(Header * h1, Header * h2) {
    int mask = 0xfff; 
    return (((uintptr_t)h1 & ~mask) == ((uintptr_t)h2 & ~mask)); 
}


int mem_init() {
    free_list = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (free_list == MAP_FAILED) {
        return FAILURE;
    } 
    
    else {
        free_list->size = PAGE_SIZE - sizeof(Header);
        free_list->next = NULL;
        free_list->previous = NULL;
        return SUCCESS;
    }
}

void mem_free(void * ptr) {
    Header * mem = get_header(ptr);
    set_free(mem);

    if (mem->next != NULL && is_free(mem->next) && same_page(mem, mem->next)) {
        mem->size += mem->next->size + sizeof(Header);         
    	mem->next = mem->next->next; 
        
        if (mem->next != NULL) {
	        mem->next->previous = mem; 
    	}	
    }

    if (mem->previous != NULL && is_free(mem->previous) && same_page(mem, mem->previous)) { 
        mem->previous->size += mem->size + sizeof(Header);         
    	mem->previous->next = mem->next; 
        
        if(mem->next != NULL) {
	        mem->next->previous = mem->previous; 
	    }     
    	mem = mem->previous;
    }

    if (mem->size == PAGE_SIZE - sizeof(Header)) {
        if (mem->previous) {	     
	        mem->previous->next = mem->next; 
	    }
	    if (mem->next) {
	        mem->next->previous = mem->previous; 
	    }
	    if (mem == free_list) {
	        free_list = mem->next;
    	}
    munmap(mem, PAGE_SIZE);
   }
}

int mem_extend(Header * last) { 
    Header * new_page = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (new_page == MAP_FAILED) {
    	return FAILURE; 
    }
    new_page->size = PAGE_SIZE - sizeof(Header); 
    new_page->next = NULL;                       
    new_page->previous = last; 

    last->next = new_page;
    return SUCCESS; 
}

void * mem_alloc(size_t requested_size) {   
    if (PAGE_SIZE - sizeof(Header) < requested_size) {
	    return NULL; 
    } 
    if (free_list == NULL) { 
    	if (mem_init()) {
    	    return NULL; 
    	}
    }
    size_t aligned_size;
    if (requested_size % WORD_SIZE) {
        aligned_size = requested_size + (WORD_SIZE - (requested_size % WORD_SIZE));
    } else {
        aligned_size = requested_size;
    }
    
    Header * current = free_list;

    while (current->next != NULL &&  (is_allocated(current) || (current->size < aligned_size))) {
	    current = current->next; 
    }
    if (is_allocated(current) || (current->size < aligned_size)) {
        if (mem_extend(current)) {
            return NULL;
        }
        current = current->next;
    }
	void * payload_start = (void *)((char *)current + sizeof(Header));

	if (current->size > (aligned_size + sizeof(Header))) {
	    Header * new_header = (Header *) ((char *)current + sizeof(Header) + aligned_size); 
	    new_header->size = current->size - aligned_size - sizeof(Header);
        if (current->next != NULL) {
            current->next->previous = new_header;
        }
        current->size = aligned_size;
        current->next = new_header; 
	    new_header->previous = current; 

	}
        set_allocated(current); 	   

   return payload_start; 
}

size_t get_size(Header *header) {
    return ((header->size >> 1) << 1);
}

void print_list() {
    Header *current = free_list;
    if (current == NULL) {
        printf("Empty list.\n");
        return;
    }
   // printf("Free List:\n");
    while (current != NULL) {
        printf("%p -> ", (void *)current);
        current = current->next;
    }
    printf("\n");
}

void print_header(Header *header) {
    if (!header) {
        printf("Header is NULL\n");
        return;
    }
    printf("Header at %p:\n", (void *)header);
    printf("  Size: %zu\n", get_size(header));
    printf("  Allocated: %d\n", is_allocated(header));
    printf("  Next: %p\n", (void *)header->next);
    printf("  Previous: %p\n", (void *)header->previous);
}
