//Authors: Kevin O'Neill - oneillhn@bc.edu, William Petrik - petrikw@bc.edu
#include <stdio.h>
#include <stdlib.h> 
#include <sys/time.h>

#define FALSE 0 
#define TRUE 1 
#define MAX_VALUE 20
#define MAX_DIM_POWER 10
#define MIN_DIM_POWER 3

void print(const int, const int * const);


void init(const int dim, int * const m) {
    for (int i = 0; i < dim * dim; ++i) {
        m[i] = rand() % MAX_VALUE;
    }
}

void multiply(const int dim, const int * const a, int * const b, int * const c) {
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            c[i * dim + j] = 0;
            for (int k = 0; k < dim; k++){
                c[i * dim + j] += a[i * dim + k] * b[k * dim + j];
            }
        }
    }
}

void transpose(const int dim, int * const m) {
    for (int i = 0; i < dim; i++) {
	    for (int j = 0; j < i; j++) {
	        int temp = m[i * dim + j];
   	        m[i * dim + j] = m[j * dim + i];
	        m[j * dim + i] = temp; 
	    }
    }
}

void multiply_transpose(const int dim, const int * const a, int * const b_t, int * const c) {
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            c[i * dim + j] = 0;
            for (int k = 0; k < dim; k++) {
                c[i * dim + j] += a[i * dim + k] * b_t[j * dim + k];
            }
        }
    }
}

void transpose_and_multiply(const int dim, const int * const a, int * const b, int * const c) {
    transpose(dim, b);
    multiply_transpose(dim, a, b, c);
}

struct timeval run_and_time(
    void (* mult_func)(const int, const int * const, int * const, int * const),
    const int dim,
    const int * const a,
    int * const b,
    int * const c
    ) {

    struct timeval start;
    gettimeofday(&start, NULL);
    mult_func(dim, a, b, c);
    struct timeval end;
    gettimeofday(&end, NULL);
    struct timeval difference;
    if (end.tv_usec < start.tv_usec) {
        end.tv_sec = end.tv_sec - 1;
        end.tv_usec = end.tv_usec + 1000000;
    }
    difference.tv_sec = end.tv_sec - start.tv_sec;
    difference.tv_usec = end.tv_usec - start.tv_usec;
    return difference;
}

int verify(const int dim, const int * const c1, const int * const c2) {
    for(int i = 0; i < dim * dim; i++) {
	if (c1[i] != c2[i]) {
	    return FALSE;
	}
    }
    return TRUE; 
} 

void run_test(const int dim) {
    printf("Testing %d-by-%d square matrices.\n", dim, dim);
    int * a = calloc(dim * dim, sizeof(int));
    int * b = calloc(dim * dim, sizeof(int));
    int * c = calloc(dim * dim, sizeof(int));
    int * d = calloc(dim * dim, sizeof(int));

    init(dim, a);
    init(dim, b);

    struct timeval multiply_test = run_and_time(&multiply, dim, a, b, c);
    struct timeval transpose_and_multiply_test = run_and_time(&multiply_transpose, dim, a, b, d);
    if (verify(dim, c, d)) {
        printf("Results agree.\n");
    } else {
        printf("Error in results.\n");
    }
    long sec = (long) multiply_test.tv_sec;
    int msec = (int) multiply_test.tv_usec;
    free(a);
    free(b);
    free(c);
    free(d);


    printf("Standard multiplication: %ld seconds, %d microseconds\n", sec, msec);
    sec = (long) transpose_and_multiply_test.tv_sec;
    msec = (int) transpose_and_multiply_test.tv_usec;
    printf("Multiplication with transpose: %ld seconds, %d microseconds\n", sec, msec);
}

void print(const int dim, const int * const m) { 
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
	    printf("%d  ", m[i * dim + j]);
	}
        printf("\n"); 
    }
    printf("\n"); 
}	

int main() {
    for (int power = MIN_DIM_POWER; power <= MAX_DIM_POWER; power++) {
        run_test(1 << power);
        printf("\n");
    }
    return EXIT_SUCCESS;
}
