//decodes a file and writes it to another file
//Authors: William Petrik - petrikw@bc.edu, Kevin O'Neill - oneillhn@bc.edu

#include "utils.h"
#include "utils.c"

void decode_chars(unsigned int input, unsigned char * const plain_chars) {
    for (int i = 0; i < CHARS_PER_INT; i++) {
        unsigned char count = ((input >> (i * COUNT_BITS)) & COUNT_MASK) % ASCII_BITS;
        unsigned char ret = (input >> (COUNTS_MASK + ASCII_BITS * i)) & ASCII_MASK;
        plain_chars[i] = (((ret >> count) | (ret << (ASCII_BITS - count))) & ASCII_MASK);
    }
}

void decode_file(const char * const input_file, const char * const output_file) {
    FILE * input = fopen_checked(input_file, "r");
    FILE * output = fopen(output_file, "w");

    int letter;
    while (fread(&letter, sizeof(letter), 1, input)) {
        unsigned char plain_chars[CHARS_PER_INT];
        decode_chars(letter, plain_chars);
        for (int i = 0; i < CHARS_PER_INT;i++) {
            fputc((char) plain_chars[i], output);
        }
    }
    fclose(output);
    fclose(input);
}

int main(int argc, char ** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: ./decode <input-ciphertext-file> <output-plaintext-file>\n");
        return EXIT_FAILURE;
    }
    decode_file(argv[1], argv[2]);
    return EXIT_SUCCESS;
}
