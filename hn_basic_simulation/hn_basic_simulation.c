/*****************************************************
 * C FILE (main): hn_basic_simulation.c              *
 * MODULE: An illustration of the functionalities    *
 *         of all the modules                        *
 *                                                   *
 * PROJECT NAME: Hopfield Network simulation         *
 *               (prototype)                         *
 *                                                   *
 * SUPERVISOR: Mark van Rossum                       *
 *                                                   *
 * AUTHOR:  Lorenzo Mella                            *
 *                                                   *
 *****************************************************/


#include "../debug_log.h"
#include "../hn_types.h"
#include "../hn_macro_utils.h"
#include "../hn_data_io.h"
#include "../hn_network.h"
#include "../hn_modes.h"
#include "../hn_parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


int main(int argc, char **argv)
{
    long num_updates;
    
    /* Data-structures */
    hn_network net;
    hn_mode_utils utils;

    spike_T *pattern = NULL;
    spike_T *pcopy = NULL;
    double **weights = NULL;
    double *overlaps = NULL;
    
    /* Seed if DEBUG_LOG is not toggled */
#   ifndef DEBUG_LOG
    srand(time(NULL));
#   endif
    
    /* Loading options from command line arguments */
    hn_options *opts;
    KillUnless((opts = malloc(sizeof (hn_options))) != NULL);
    KillUnless(hn_retrieve_options(opts, argc, argv));
    
    /* Set warning threshold */
    size_t warning_threshold = opts->max_units;
    
    /* Allocate and retrieve weight matrix from file (default: ./weights.bin) */
    printf("Reading weight matrix from file: %s\n", opts->w_filename);
    MatrixAlloc(weights, opts->max_units, opts->max_units);
    KillUnless(hn_read_weights(weights, opts->w_filename, opts->max_units) != IOFailure);
    printf("... done!\n");
    
    /* Initialise update mode (default: SEQUENTIAL) */
    utils = hn_utils_with_mode(opts->mode);
    
    /* Allocate array holding information to be saved */
    KillUnless((overlaps = malloc(opts->max_patterns * sizeof (double))) != NULL);
    
    /* Program description to the user */
    printf("\n- Hopfield Network simulation -\n\n"
           "Number of units (max_units) = %lu\n"
           "Number of patterns to test (max_patterns) = %lu\n\n",
           opts->max_units, opts->max_patterns);
    
    /* Main loop: test all patterns in sequence */
    for (size_t n = 0; n < opts->max_patterns; ++n) {
        
        Logger("Pattern %lu: press key to continue\n", n);
#       ifdef DEBUG_LOG
        getchar();
#       endif
        
        /* Load the next (n-th) initial pattern */
        printf("Reading pattern %lu...\n", n + 1);
        KillUnless((pattern = malloc(opts->max_units * sizeof (spike_T))) != NULL);
        KillUnless(hn_read_next_pattern(pattern, opts->p_filename, opts->max_units) !=
		   IOFailure);
        printf("... done!\n");
        
        /* Generate network structure with weights and the extracted pattern */
        net = hn_network_from_params(weights, opts->threshold, pattern);
        
        /* Copy the array for later comparison */
        pcopy = hn_pattern_copy(pattern, opts->max_units);
        
        /* Determine the stable pattern, using the initial pattern
         * stored in net. A Pointer to net.activations is returned hence
         * we don't really need to use it */ 
        num_updates = hn_test_pattern(net, NULL, opts->max_units, warning_threshold, utils);
        
        /* Count the absolute frequency of matches between stable
         * and initial pattern */
        overlaps[n] = hn_overlap_frequency(net.activations, pcopy, opts->max_units);
        
        printf("Pattern %lu: overlaps = %g; updates before convergence = %ld\n",
	       n + 1, overlaps[n], num_updates);
        
        free(pcopy);
        pcopy = NULL;
        free(net.activations);
        net.activations = NULL;
    }

    size_t bytes_written;

    printf("Saving overlaps on file \'%s\'...\n", opts->s_filename);
    KillUnless(hn_save(overlaps, opts->s_filename, opts->max_patterns, &bytes_written)
               != IOFailure);
    printf("done! (size: %lu bytes)\n\n", bytes_written);
    
    /* Cleanup */
    free(overlaps);
    MatrixFree(weights);
    free(opts);
    
    exit(EXIT_SUCCESS);
}
