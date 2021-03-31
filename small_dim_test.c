#include "hn_types.h"
#include "hn_macro_utils.h"
#include "hn_modes.h"
#include "hn_network.h"
#include "hn_data_io.h"
#include "debug_log.h"


#include <stdlib.h>
#include <stdio.h>


int main(int argc, char **argv)
{
    /* Data and parameters */
    double threshold = 0.;
    size_t max_units = 3;
    
    char w_filename[] = "example_data_files/small_weights.bin";
    char p_filename[] = "example_data_files/small_patterns.bin";
    
    /* Data-structures */
    hn_mode_utils utils = hn_utils_with_mode(MODE_SEQUENTIAL);
    hn_network net;
    
    double **weights;
    MatrixAlloc(weights, max_units, max_units);

    spike_T *pattern = malloc(max_units * sizeof(spike_T));
    KillUnless(pattern != NULL);
    
    enum io_error_code io_err = hn_read_weights(weights, w_filename, max_units);
    KillUnless(io_err != IOFailure);
    
    /* Main loop (notice that hn_read_next_pattern() overwrites
     * the previous pattern */
    while (hn_read_next_pattern(pattern, p_filename, max_units) != IOFailure) {
        long unsigned timesteps;
        
        printf("Initial state:\n");
        for (int i = 0; i < max_units; ++i) {
            printf("%d ", pattern[i]);
        }
        puts("");
        
        net = hn_network_from_params(weights, threshold, pattern);
        timesteps = hn_test_pattern(net, NULL, max_units, max_units, utils);
        
        printf("Final state:\n");
        for (int i = 0; i < max_units; ++i) {
            printf("%d ", pattern[i]);
        }
        printf("\nTimesteps taken: %lu\n\n", timesteps);
    }
    
    free(pattern);
    MatrixFree(weights);    
    exit(EXIT_SUCCESS);
}
