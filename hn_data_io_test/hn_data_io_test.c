/* hn_data_io.test */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../../debug_log/debug_log.h"
#include "../hn_types.h"
#include "../hn_data_io.h"
#include "../hn_network.h"

#define PrintArr(arr, length)                                                  \
        do {                                                                   \
            int PRINT_ARR_I;                                                   \
            for (PRINT_ARR_I = 0; PRINT_ARR_I < (length); ++PRINT_ARR_I) {     \
                printf("%+.2g ", (double)(arr)[PRINT_ARR_I]);                  \
            }                                                                  \
            putchar('\n');                                                     \
        } while(0)


void weight_read_test(char *w_filename, size_t max_units)
{
    printf("weight_read_test\n");
    
    /* Allocate memory for weight matrix */
    double **weights = NULL;
    KillUnless((weights = malloc(max_units * sizeof (double *))) != NULL);
    for (size_t i = 0; i < max_units; ++i) {
        KillUnless((weights[i] = malloc(max_units * sizeof (double))) != NULL);
    }
    
    /* Load weight matrix */
    KillUnless(IOFailure != (hn_read_weights(weights, w_filename, max_units)));
    
    /* Print weight matrix line by line */
    for (size_t i = 0; i < max_units; ++i) {
        PrintArr(weights[i], max_units);
    }
    
    /* Free weight memory */
    for (size_t i = 0; i < max_units; ++i) {
        free(weights[i]);
    }
    free(weights);
}


void pattern_read_test(char *p_filename, size_t max_patterns, size_t max_units)
{   
    printf("pattern_read_test\n");

    spike_T *pattern = NULL;   
    for (size_t n = 0; n < max_patterns; ++n) {
        /* Allocate memory for pattern */
        KillUnless(NULL != (pattern = malloc(max_units * sizeof (size_t))));
        
        /* Load next pattern */
        KillUnless(IOFailure != (hn_read_next_pattern(pattern, p_filename,
               max_units)));
        
        /* Print pattern */
        PrintArr(pattern, max_units);
    }
    
    /* Free pattern memory */
    free(pattern);
}


size_t rand_pattern_test(double coding_level, size_t max_units)
{
    printf("rand_pattern_test\n");
    
    spike_T *rand_pattern = malloc(max_units * sizeof (spike_T));
    KillUnless(rand_pattern != NULL);

    /* Filling pattern at random with specified coding level */
    hn_fill_rand_pattern(rand_pattern, coding_level, max_units);
    PrintArr(rand_pattern, max_units);

    /* Counting spiking units */
    size_t spiking_units = 0;
    for (size_t i = 0; i < max_units; ++i) {
        spiking_units += rand_pattern[i] == +1;
    }
    free(rand_pattern);   
    return spiking_units;
}


void hebb_weight_test()
{
    printf("hebb_weight_test\n");
    
    /* Create a pattern with integers and one with squares */
    spike_T **patterns;
    KillUnless((patterns = malloc(2 * sizeof (spike_T *))) != NULL);
    KillUnless((patterns[0] = malloc(5 * sizeof (spike_T))) != NULL);
    KillUnless((patterns[1] = malloc(5 * sizeof (spike_T))) != NULL);
    
    for (size_t j = 0; j < 5; ++j) {
        patterns[0][j] = j + 1;
        patterns[1][j] = j * j;
    }

    /* Prepare weight matrix */
    double **weights = NULL;
    KillUnless((weights = malloc(5 * sizeof (double *))) != NULL);
    for (size_t i = 0; i < 5; ++i) {
        KillUnless((weights[i] = malloc(5 * sizeof (double))) != NULL);
    }

    /* "Learn" weight matrix given the two patterns */
    printf("Weights from {1,2,3,4,5} and {0,1,4,9,16}\n");
    hn_hebb_weights_from_patterns(weights, patterns, 2, 5, 0);
    for (size_t i = 0; i < 5; ++i) {
        PrintArr(weights[i], 5);
    }

    /* Same but with diagonal suppression */
    printf("Weights from {1,2,3,4,5} and {0,1,4,9,16} "
           "(with diagonal suppression)\n");
    hn_hebb_weights_from_patterns(weights, patterns, 2, 5, 1);
    for (size_t i = 0; i < 5; ++i) {
        PrintArr(weights[i], 5);
    }
    
    /* Learn weights incrementally */
    printf("Weights from {1,2,3,4,5} and {0,1,4,9,16} (incremental)\n");
    /* Set weights to 0 */
    for (size_t i = 0; i < 5; ++i) {
        for (size_t j = 0; j < 5; ++j) {
            weights[i][j] = 0.;
        }
    }
    hn_hebb_weights_increment_with_pattern(weights, patterns[0], 5, 0);
    hn_hebb_weights_increment_with_pattern(weights, patterns[1], 5, 0);
    for (size_t i = 0; i < 5; ++i) {
        PrintArr(weights[i], 5);
    }

    /* Learn weights incrementally with diagonal suppression */
    printf("Weights from {1,2,3,4,5} and {0,1,4,9,16} "
           "(incremental with diagonal suppression)\n");
    /* Set weights to 0 */
    for (size_t i = 0; i < 5; ++i) {
        for (size_t j = 0; j < 5; ++j) {
            weights[i][j] = 0.;
        }
    }
    hn_hebb_weights_increment_with_pattern(weights, patterns[0], 5, 1);
    hn_hebb_weights_increment_with_pattern(weights, patterns[1], 5, 1);
    for (size_t i = 0; i < 5; ++i) {
        PrintArr(weights[i], 5);
    }
}


int main(int argc, char **argv)
{
    size_t max_units = 20;
    size_t max_patterns = 5;

    /* These files SHOULD BE already in the folder hn_data_io_test */
    char *w_filename = "weightsNR.bin";
    char *p_filename = "patternsNR.bin";
    
    /* Read tests: a matrix whose rows are constant, 1 through 20
     * and alternated all +1/all -1 patterns are to be visualised */
    weight_read_test(w_filename, max_units);
    pattern_read_test(p_filename, max_patterns, max_units);
    
    /* Random pattern test */
    srand(0);
    size_t total_selective_neurons = 0;
    for (size_t trial = 0; trial < max_patterns; ++trial) {
        total_selective_neurons += rand_pattern_test(.25, max_units);
    }
    printf("Average +1 units = %.2f (theoretical = 5)\n",
           (double)total_selective_neurons/max_patterns);
    
    /* Hebb rule weight creation test */
    hebb_weight_test();
    
    exit(EXIT_SUCCESS);
}
