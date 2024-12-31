//Encodes a text file into a series of numbers
//Authors: William Petrik - petrikw@bc.edu, Kevin O'Neill - oneillhn@bc.edu

#include "utils.h"
#include "utils.c"
 
unsigned char ones(unsigned char input) {
    unsigned char ret = 0x00;
    while (input) { 
	   ret += input & 1; 
	   input >>= 1; 
    }
    return ret; 
}

unsigned char rotate(unsigned char plain, const unsigned char count) {
    unsigned char cipher = 0;
    for (int pos = 0; pos < ASCII_BITS; ++pos, plain >>= 1) {
        cipher |= ((plain & 1) << ((pos + count) % ASCII_BITS));
    }
    return cipher; 
} 

unsigned int encode_chars(const unsigned char * const plain_chars) {
    unsigned int result = 0; 
    for (int i = 2; i >= 0; i--) {
	    unsigned char count = ones(plain_chars[i]); 
       	result = result | (((unsigned int) (rotate(plain_chars[i], count) & ASCII_MASK)) << (i * ASCII_BITS + COUNTS_MASK)); 
	    result = result | ((unsigned int) (count & COUNT_MASK) << (i * COUNT_BITS));		
    }
    return result; 
}

void encode_file(const char * const input_file, const char * const output_file) {
    FILE * input = fopen_checked(input_file, "r");
    FILE * output = fopen(output_file, "w");

    unsigned char group[CHARS_PER_INT] = {0};   
    int count = 0; 

    while (1) {
        int letter = fgetc(input);
    	if (letter == EOF){
	    	break;
    	}

	    group[count] = letter; 
    	count++;

	    if (count==CHARS_PER_INT) {
	        unsigned int encoded_group = encode_chars(group);
 	        fwrite(&encoded_group, sizeof(encoded_group), 1, output);
	        count=0; 
	    }
    }

    if (count > 0) {
        for (int i = count; i < CHARS_PER_INT; i++) {
            group[i] = '0'; 
        }
	    unsigned int encoded_group = encode_chars(group);
 	    fwrite(&encoded_group, sizeof(encoded_group), 1, output);
    }

    fclose(input);
    fclose(output);
}

int main(int argc, char ** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: ./encode <input-plaintest-file> <output-ciphertext-file>\n");
        return EXIT_FAILURE;
    }
    encode_file(argv[1], argv[2]);
    return EXIT_SUCCESS;       
}
