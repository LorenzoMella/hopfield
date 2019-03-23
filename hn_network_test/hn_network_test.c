/*****************************************************
 * C FILE (main): hn_network_test.c                  *
 * MODULE: Main application (test)                   *
 *                                                   *
 * PROJECT NAME: Hopfield Network simulation         *
 *               (prototype)                         *
 *                                                   *
 * SUPERVISOR: Mark van Rossum                       *
 *                                                   *
 * AUTHOR:  Lorenzo Mella                            *
 * VERSION: 09/07/2016                               *
 *                                                   *
 *****************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../hn_types.h"   /* define DEBUG_LOG here to debug across everything */
#include "../hn_data_io.h"
#include "../hn_network.h"
#include "../hn_modes.h"
#include "../hn_macro_utils.h"
#include "../../debug_log/debug_log.h"

#define MAX_SIZE 500
#define MAX_NUM_PATTERNS 5

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

int main(int argc, char **argv)
{
    size_t max_units = MAX_SIZE;
    size_t max_patterns = MAX_NUM_PATTERNS;
    size_t warning_threshold = max_units;
    double threshold = 0.0;
    
    printf("\n- Hopfield Network simulation -\n\n"
           "Number of units (MAX_SIZE) = %d\n"
           "Number of patterns to test (MAX_NUM_PATTERNS) = %d\n\n",
           MAX_SIZE, MAX_NUM_PATTERNS);

    /* Create the full path string for the data-files to load
     * and define default paths */
    char full_path[PATH_MAX + 1];
    char default_weights_path[] = "../example_data_files/weights500.bin";
    char default_patterns_paths[] = "../example_data_files/patterns500.bin";
    
    int opt_c = getopt(argc, argv, "w:");
    Logger("opt_c = '%c', filename = \"%s\"\n", opt_c, optarg);
    snprintf(full_path, PATH_MAX, "%s",
	     opt_c != -1 ? optarg : default_weights_path);
    Logger("full_path = \"%s\"\n", full_path);
    
    /* 2D array memory allocation for the weight matrix */
    double **weights;
    MatrixAlloc(weights, max_units, max_units);

    printf("DEBUG_LOG = %s\n", DEBUG_LOG);
    
    /* Load weight matrix from file */
    KillUnless(hn_read_weights(weights, full_path, max_units) != IOFailure);

    /* Create the full path string for the pattern data-file */
    opt_c = getopt(argc, argv, "p:");
    Logger("opt_c = '%c', filename = \"%s\"\n", opt_c, optarg);
    snprintf(full_path, PATH_MAX, "%s",
	     opt_c != -1 ? optarg : default_patterns_paths);
    Logger("full_path = \"%s\"\n", full_path);

    hn_network net;
    for (size_t n = 0; n < max_patterns; ++n) {
        
        Logger("Pattern %zu: press key to continue\n", n);
        #ifdef DEBUG_LOG
        getchar();
        #endif /* DEBUG_LOG */
        
        /* Load the next (n-th) initial pattern */
	spike_T *pattern;
	KillUnless((pattern = malloc(max_units * sizeof (spike_T))) != NULL);
        KillUnless(hn_read_next_pattern(pattern, full_path, max_units) != IOFailure);
        
        /* Generate network structure with weights and the extracted pattern */
        net = hn_network_from_params(weights, threshold, pattern);
        
        /* Copy the array for later comparison */
        spike_T *pcopy = hn_pattern_copy(pattern, max_units);
        
        /* Initialise network with parameters and use sequential mode */
        hn_mode_utils utils = hn_utils_with_mode(Random);
        
        /* Determine the stable pattern, using the initial pattern
         * stored in net. A Pointer to net.activations is returned hence
         * we don't really need to use it */ 
        hn_test_pattern(net, NULL, max_units, warning_threshold, utils);

	// This test should always succeed
        KillUnless(net.activations != NULL);

        /* Count the absolute frequency of matches between stable
         * and initial pattern */
        printf("Pattern %zu: #overlaps = %zu\n",
	       n, hn_overlap_frequency(net.activations, pcopy, max_units));
        
        free(net.activations);
	net.activations = NULL;
        free(pcopy);
    }
    /* Cleanup */
    hn_free(net);
    exit(EXIT_SUCCESS);
}
