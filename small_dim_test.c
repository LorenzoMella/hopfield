#include <stdlib.h>
#include <stdio.h>
#include "hn_types.h"
#include "hn_macro_utils.h"
#include "hn_modes.h"
#include "hn_network.h"
#include "hn_data_io.h"
#define DEBUG_LOG
#include "debug_log.h"

int main(int argc, char **argv)
{
    /* Indices */
    int i;
    
    /* Data and parameters */
    double **weights = NULL;
    spike_T *pattern = NULL;
    double threshold = 0.;
    size_t max_units = 3;
    
    /* I/O declarations */
    io_error_code io_err;
    char *w_filename = "example_data_files/small_weights.bin";
    char *p_filename = "example_data_files/small_patterns.bin";
    
    /* Data-structures */
    hn_mode_utils utils = hn_utils_with_mode(SEQUENTIAL);
    hn_network net;
    
    hn_matrix_alloc(weights, max_units, max_units);
    pattern = malloc(max_units * sizeof (spike_T));
    exit_on_exception(pattern);
    
    io_err = hn_read_weights(weights, w_filename, max_units);
    exit_on_exception(IO_FAILURE != io_err);
    
    /* Main loop (notice that hn_read_next_pattern() overwrites the previous
     * pattern but doesn't allocate new memory: free is needed only at the
     * end of main; the same holds for hn_test_pattern()) */
    while (IO_FAILURE != hn_read_next_pattern(pattern, p_filename, max_units)) {
        long unsigned timesteps;
        
        printf("Initial state:\n");
        for (i = 0; i < max_units; ++i) {
            printf("%d ", pattern[i]);
        }
        putchar('\n');
        
        net = hn_network_from_params(weights, threshold, pattern);
        timesteps = hn_test_pattern(net, NULL, max_units, max_units, utils);
        
        printf("Final state:\n");
        for (i = 0; i < max_units; ++i) {
            printf("%d ", pattern[i]);
        }
        printf("\nTimesteps taken: %lu\n\n", timesteps);
    }
    
    free(pattern);
    hn_matrix_free(weights);
    
    exit(EXIT_SUCCESS);
}