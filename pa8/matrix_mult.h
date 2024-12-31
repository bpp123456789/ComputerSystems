/* PA8: Parallel Matrix Multiplication
 * Authors: Kevin O'Neill and William Petrik 
 */


#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

#define DIM 1024
#define NUM_WORKERS 4 
#define SUCCESS 0
#define FAILURE -1

typedef void (* multiply_function)(
    const double * const,  
    const double * const,  
    double * const,        
    const int,            
    const int);

typedef struct Args {
    const double * a;
    const double * b;
    double * c;
    int row_start;
    int chunk;
    int dim;
} Args;

void init_matrix(double *, int);
void multiply_serial(const double * const, const double * const, double * const, const int, const int); 
void print_matrix(double *, int); 
void multiply_chunk(const double * const, const double * const, double * const, const int, const int, const int); 
void munmap_checked(void *, size_t);
void * mmap_checked(size_t);
int fork_checked(); 
void multiply_parallel_processes(const double * const, const double * const, double * const, const int, const int);
int verify(const double * const, const double * const, const int); 
struct timeval time_diff(struct timeval *, struct timeval *); 
void print_elapsed_time(struct timeval *, struct timeval *, const char * const); 
void print_verification(const double * const, const double * const, const int, const char * const); 
void run_and_time(
        multiply_function multiply_matrices,
        const double * const,
        const double * const,
        double * const,
        const double * const,
        const int,
        const char * const,
        const int,
        const bool);
void * task(void *);
void multiply_parallel_threads(const double * const, const double * const, double * const, const int, const int);
