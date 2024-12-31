//author: William Petrik - petrikw@bc.edu
//author: Kevin O'Neill - oneillhn@bc.edu

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BINS 64
#define CENTER_BIN_INDEX BINS / 2 + 1
#define HISTOGRAM_SPAN 0.05
#define RUNS 50000
#define SAMPLES 10000
#define SCALE 32

double get_mean_of_uniform_random_samples() {
    double sum = 0;
    for (int i = 0; i < SAMPLES; i++) {
        sum += (((double) rand()) / (double) RAND_MAX) * 2 - 1;
    }
    return sum / (double) SAMPLES;
}

double populate_values_and_get_mean(double * values) {
    double sum = 0;
    for (int i = 0; i < RUNS; i++) {
        values[i] = get_mean_of_uniform_random_samples();
        sum += values[i];
    }
    return sum / RUNS;
}

double get_mean_squared_error(double * values, double mean) {
    double sum = 0;
    for (int y = 0; y < RUNS; y++) {
        sum += (values[y] - mean) * (values[y] - mean);
    }
    return sum / RUNS;
}

void create_histogram(double * values, int * counts) {
    memset(counts, 0, BINS * sizeof(int));
    double bin_size = HISTOGRAM_SPAN / (double) BINS;
    for (int i = 0; i < RUNS; i++) { 
        int index = (int) (values[i] / bin_size + CENTER_BIN_INDEX);
        counts[index]++;
    }
}	

void print_histogram(int * counts) {
	double bin_start = -1 * (HISTOGRAM_SPAN / 2);
	double bin_size = HISTOGRAM_SPAN / (double) BINS;
	double label = bin_start + bin_size / 2;
	for (int i = 0; i < BINS; i++) {
		printf("%.4f  ", label);
		int x_count = counts[i]/SCALE;
		for (int i = 0; i < x_count; i++) {
		       printf("X");
		}
		printf("\n");
        label += bin_size;
	}
}

int main(int argc, char * argv[]) {
    double mean;
    double * values;
    if (argc > 1 && strcmp(argv[1], "test") == 0) {
        srand(1);
        printf("%s\n", "test");
    } else {
        srand((int)time(NULL));
    }
    values = calloc(RUNS, sizeof(double));
    mean = populate_values_and_get_mean(values);
    double error = get_mean_squared_error(values, mean);
    int counts[BINS];
    create_histogram(values, counts);
    free(values);
    print_histogram(counts);
    printf("Sample mean: %f   Sample variance: %f\n", mean, error);
    return EXIT_SUCCESS;
}
