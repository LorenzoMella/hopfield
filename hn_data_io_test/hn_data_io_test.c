/* hn_data_io.test */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "hn_types.h"
#include "hn_data_io.h"
#include "hn_network.h"

#define PRINT_ARR(arr, length)                                                 \
        do {                                                                   \
            int PRINT_ARR_I;                                                   \
            for (PRINT_ARR_I = 0; PRINT_ARR_I < (length); ++PRINT_ARR_I) {     \
                printf("%+.2g ", (double)(arr)[PRINT_ARR_I]);                  \
            }                                                                  \
            putchar('\n');                                                     \
        } while(0)


void weight_read_test(char *w_filename, size_t max_units);
void pattern_read_test(char *p_filename, size_t max_patterns, size_t max_units);
size_t rand_pattern_test(double coding_level, size_t max_units);
void hebb_weight_test();


int main(int argc, char **argv)
{
    size_t max_units = 20;
    size_t max_patterns = 5;
    size_t trial;
    size_t max_trials = 20;
    /* These files SHOULD BE already in the folder hn_data_io_test */
    char *w_filename = "weightsNR.bin";
    char *p_filename = "patternsNR.bin";
    size_t total_selective_neurons;
    
    /* Read tests: a matrix whose rows are constant, 1 through 20
     * and alternated all +1/all -1 patterns are to be visualised */
    weight_read_test(w_filename, max_units);
    pattern_read_test(p_filename, max_patterns, max_units);
    
    /* Random pattern test */
    srand(0);
    total_selective_neurons = 0;
    for (trial = 0; trial < max_patterns; ++trial) {
        total_selective_neurons += rand_pattern_test(.25, max_units);
    }
    printf("Average +1 units = %.2f (theoretical = 5)\n",
           (double)total_selective_neurons/max_patterns);
    
    /* Hebb rule weight creation test */
    hebb_weight_test();
    
    exit(EXIT_SUCCESS);
}


void weight_read_test(char *w_filename, size_t max_units)
{
    size_t i;
    double **weights = NULL;
    
    printf("weight_read_test\n");
    /* Allocate memory for weight matrix */
    assert(NULL != (weights = malloc(max_units * sizeof (double *))));
    for (i = 0; i < max_units; ++i) {
        assert(NULL != (weights[i] = malloc(max_units * sizeof (double))));
    }
    
    /* Load weight matrix */
    assert(IO_FAILURE != (hn_read_weights(weights, w_filename, max_units)));
    
    /* Print weight matrix line by line */
    for (i = 0; i < max_units; ++i) {
        PRINT_ARR(weights[i], max_units);
    }
    
    /* Free weight memory */
    for (i = 0; i < max_units; ++i) {
        free(weights[i]);
    }
    free(weights);
}


void pattern_read_test(char *p_filename, size_t max_patterns, size_t max_units)
{
    size_t n;
    spike_T *pattern = NULL;
    
    printf("pattern_read_test\n");
    
    for (n = 0; n < max_patterns; ++n) {
        /* Allocate memory for pattern */
        assert(NULL != (pattern = malloc(max_units * sizeof (size_t))));
        
        /* Load next pattern */
        assert(IO_FAILURE != (hn_read_next_pattern(pattern, p_filename,
               max_units)));
        
        /* Print pattern */
        PRINT_ARR(pattern, max_units);
    }
    
    /* Free pattern memory */
    free(pattern);
}


size_t rand_pattern_test(double coding_level, size_t max_units)
{
    size_t i;
    size_t spiking_units = 0;
    
    printf("rand_pattern_test\n");
    
    spike_T *rand_pattern = malloc(max_units * sizeof (spike_T));
    assert(rand_pattern != NULL);
    
    hn_fill_rand_pattern(rand_pattern, coding_level, max_units);
    PRINT_ARR(rand_pattern, max_units);
    
    for (i = 0; i < max_units; ++i) {
        spiking_units += rand_pattern[i] == +1;
    }
    
    free(rand_pattern);
    
    return spiking_units;
}


void hebb_weight_test()
{
    size_t i, j;
    spike_T **patterns;
    double **weights;
    
    printf("hebb_weight_test\n");
    
    weights = malloc(5 * sizeof (double *));
    for (i = 0; i < 5; ++i) {
        weights[i] = malloc(5 * sizeof (double));
    }
    
    /* Create a pattern with integers and one with squares */
    patterns = malloc(2 * sizeof (spike_T *));
    patterns[0] = malloc(5 * sizeof (spike_T));
    patterns[1] = malloc(5 * sizeof (spike_T));
    
    for (j = 0; j < 5; ++j) {
        patterns[0][j] = j+1;
        patterns[1][j] = j*j;
    }
    
    printf("Weights from {1,2,3,4,5} and {0,1,4,9,16}\n");
    hn_hebb_weights_from_patterns(weights, patterns, 2, 5, 0);
    for (i = 0; i < 5; ++i) {
        PRINT_ARR(weights[i], 5);
    }
    
    printf("Weights from {1,2,3,4,5} and {0,1,4,9,16} "
           "(with diagonal suppression)\n");
    hn_hebb_weights_from_patterns(weights, patterns, 2, 5, 1);
    for (i = 0; i < 5; ++i) {
        PRINT_ARR(weights[i], 5);
    }
    
    /* Try to obtain the above incrementally */
    printf("Weights from {1,2,3,4,5} and {0,1,4,9,16} (incremental)\n");
    /* Set weights to 0 */
    for (i = 0; i < 5; ++i) {
        for (j = 0; j < 5; ++j) {
            weights[i][j] = 0.;
        }
    }
    
    hn_hebb_weights_increment_with_pattern(weights, patterns[0], 5, 0);
    hn_hebb_weights_increment_with_pattern(weights, patterns[1], 5, 0);
    for (i = 0; i < 5; ++i) {
        PRINT_ARR(weights[i], 5);
    }
    
    printf("Weights from {1,2,3,4,5} and {0,1,4,9,16} "
           "(incremental with diagonal suppression)\n");
    /* Set weights to 0 */
    for (i = 0; i < 5; ++i) {
        for (j = 0; j < 5; ++j) {
            weights[i][j] = 0.;
        }
    }
    
    hn_hebb_weights_increment_with_pattern(weights, patterns[0], 5, 1);
    hn_hebb_weights_increment_with_pattern(weights, patterns[1], 5, 1);
    for (i = 0; i < 5; ++i) {
        PRINT_ARR(weights[i], 5);
    }

}