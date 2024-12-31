//Authors Kevin O'Neill and William Petrik 

#include "matrix_mult.h" 

void init_matrix(double * matrix, int dim) {
    int x = 1; 
    for (int i = 0; i < dim; i++) {
	    for (int j = 0; j < dim; j++) {
	        matrix[i*dim+j] = x; 
	        x++; 
	    }
    }
}

void multiply_serial(const double * const a, const double * const b, double * const c, const int dim, const int num_workers) {
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            c[i * dim + j] = 0;
            for (int k = 0; k < dim; k++){
                c[i * dim + j] += a[i * dim + k] * b[k * dim + j];
            }
        }
    }
}

void print_matrix(double * matrix, int dim) {
 for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
	    printf("%f  ", matrix[i * dim + j]);
	}
        printf("\n"); 
    }
    printf("\n"); 
}

void multiply_chunk(const double * const a, const double * const b, double * const c, const int dim, const int chunk_size, const int row_start) {
   for (int i = row_start; i < row_start + chunk_size; i++) {
       for (int j = 0; j < dim; j++) {
           c[i * dim + j] = 0;
     	   for (int k = 0; k < dim; k++){
               c[i * dim + j] += a[i * dim + k] * b[k * dim + j];
           }
       }
   }
} 
void munmap_checked(void * address, size_t size) {
    int check = munmap(address, size);
    if (check == FAILURE) {
        perror("Failure to unmap");
    }
}

void * mmap_checked(size_t size) {
    void * mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        perror("Error in mapping");
        exit(EXIT_FAILURE);
    }
    return mem;
}

        
int fork_checked() {
    int process = fork();
    if (process == FAILURE) {
        perror("Failure in forking");
        exit(EXIT_FAILURE);
    }
    return process;
}

void multiply_parallel_processes(const double * const a, const double * const b, double * const c, const int dim, const int num_workers) {
    int num_procs = num_workers - 1;
    int chunk = dim / num_workers;
    int i = 0;
    double * shared_memory = mmap_checked(dim * dim * sizeof(double));
    for (i = 0; i < num_procs; ++i) {
        int pid = fork_checked();
        if (!pid) {
            multiply_chunk(a, b, shared_memory, dim, chunk, i * chunk);
            exit(EXIT_SUCCESS);
        }
    }
    while (wait(NULL) > 0);
    int what_is_left_start = i * chunk;
    int chunk_size_left = dim - (i * chunk);
    multiply_chunk(a, b, shared_memory, dim, chunk_size_left, what_is_left_start);

    for (int index = 0; index < dim * dim; index++) {
        c[index] = shared_memory[index];
    }
    munmap_checked(shared_memory, dim * dim * sizeof(double)); 
}

int verify(const double * const m1, const double * const m2, const int dim) {
    for(int i = 0; i < dim * dim; i++) {
        if (m1[i] != m2[i]) {
	        return FAILURE;
	    }
    }
    return SUCCESS; 
}

struct timeval time_diff(struct timeval * start, struct timeval * end) {
    struct timeval difference;
    if (end->tv_usec < start->tv_usec) {
        end->tv_sec = end->tv_sec - 1;
        end->tv_usec = end->tv_usec + 1000000;
    }
    difference.tv_sec = end->tv_sec - start->tv_sec;
    difference.tv_usec = end->tv_usec - start->tv_usec;
    return difference;
}

void print_elapsed_time(struct timeval * start, struct timeval * end, const char * const name) {
    printf("Time elapsed for %s:\n", name);
    struct timeval elapsed = time_diff(start, end);
    time_t seconds = elapsed.tv_sec;
    suseconds_t microseconds = elapsed.tv_usec;
    if (seconds == 1) {
        printf("1 second and ");
    } 
    else {
        printf("%ld seconds and ", seconds);
    }
    if (microseconds == 1) {
        printf("1 microsecond.\n");
    } 
    else {
        printf("%ld microseconds.\n", (long)microseconds);
    }
}

void print_verification(const double * const m1, const double * const m2, const int dim, const char * const name) {
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
        printf("%f  ", m1[i * dim + j]);
    }
        printf("\n");
    }
    printf("\n");
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
        printf("%f  ", m2[i * dim + j]);
    }
        printf("\n");
    }
    printf("\n");
}

void run_and_time(
    multiply_function multiply_matrices,
    const double * const a,
    const double * const b,
    double * const c,
    const double * const gold,
    const int dim,
    const char * const name,
    const int num_workers,
    const bool verify
    ) {
  
    struct timeval start, end;
    if (num_workers > 1) {
        printf("Algorithm: %s with %d workers.\n", name, num_workers);
    } 
    else {
        printf("Algorithm: %s with %d worker.\n", name, num_workers);
    }
    gettimeofday(&start, NULL);
    multiply_matrices(a, b, c, dim, num_workers);
    gettimeofday(&end, NULL);
    print_elapsed_time(&start, &end, name);
    printf("------------------------------\n");
}


void * task(void * arg) {
    Args * args = arg;
    multiply_chunk(args->a, args->b, args->c, args->dim, args->chunk, args->row_start);
    return NULL;
}

void multiply_parallel_threads(const double * const a, const double * const b, double * const c, const int dim, const int num_workers) {
    int num_threads = num_workers - 1;
    int chunk = dim / num_workers;
    Args args;
    args.a = a;
    args.b = b;
    args.c = c;
    args.chunk = chunk;
    args.dim = dim;
    Args arg_set[num_workers];
    pthread_t threads[num_threads];   

    int row_start = 0;
    int id; 
    for (id = 0; id < num_workers; ++id) {
        arg_set[id] = args;
   	    arg_set[id].row_start = row_start;
        row_start += chunk;
    }

    for (int i = 0; i < num_workers; ++i) {
	    pthread_create(&threads[i], NULL, task, (void *)&arg_set[i]);
    }
    
    arg_set[id].chunk = dim - arg_set[id].row_start;

    task((void *)&arg_set[num_workers-1]);

    for (int i = 0; i < num_threads; i++) {
       pthread_join(threads[i], NULL);
   }
}
