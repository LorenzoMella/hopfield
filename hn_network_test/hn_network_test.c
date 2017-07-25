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
#include <assert.h>
#include <unistd.h>
#include "hn_types.h"   /* define DEBUG_LOG here to debug across everything */
#include "hn_data_io.h"
#include "hn_network.h"
#include "hn_modes.h"
#define DEBUG_LOG  /* Uncomment here to activate DEBUG_LOG on this file only */
#include "debug_log.h"

#define MAX_SIZE 500
#define MAX_NUM_PATTERNS 5

/* We have a temporary and unsafe getopt coded here */

int main(int argc, char **argv)
{
    size_t i;
    size_t n;
    size_t overlap;
    size_t max_units = MAX_SIZE;
    size_t max_patterns = MAX_NUM_PATTERNS;
    size_t warning_threshold = max_units;
    double threshold = 0.0;
    hn_mode_utils utils;
    hn_network net;
    spike_T *pattern = NULL;
    spike_T *pcopy = NULL;
    double **weights = NULL;
    
    char opt_c;
    char folder_path[100];
    char full_path[100];
    
    printf("\n- Hopfield Network simulation -\n\n"
           "Number of units (MAX_SIZE) = %d\n"
           "Number of patterns to test (MAX_NUM_PATTERNS) = %d\n\n",
           MAX_SIZE, MAX_NUM_PATTERNS);
    
    /* Create the string holding the full path to the data-file folder */
    strcpy(folder_path, getenv("HOME"));
    strcat(folder_path, "/cprojects/hopfield/example_data_files/");
    
    debug_log("Path to data-file folder: %s\n", folder_path);
    
    /* 2D array memory allocation for the weight matrix */
    assert((weights = malloc(max_units * sizeof (double *))) != NULL);
    for (i = 0; i < max_units; ++i) {
        assert((weights[i] = malloc(max_units * sizeof (double))) != NULL);
    }
    
    /* Create the full path string for the weights data-file */
    opt_c = getopt(argc, argv, "w:");
    debug_log("opt_c = '%c', filename = \"%s\"\n", opt_c, optarg);
    strcpy(full_path, folder_path);
    if (opt_c != -1) {
        strcat(full_path, optarg);
    } else {
        strcat(full_path, "weights500.bin");
    }
    debug_log("full path = \"%s\"\n", full_path);
    
    /* Load weight matrix from file */
    exit_on_exception(hn_read_weights(weights, full_path, max_units)
                      != IO_FAILURE);
    /* Create the full path string for the pattern data-file */
    opt_c = getopt(argc, argv, "p:");
    debug_log("opt_c = '%c', filename = \"%s\"\n", opt_c, optarg);
    strcpy(full_path, folder_path);
    if (opt_c != -1) {
        strcat(full_path, optarg);
    } else {
        strcat(full_path, "patterns500.bin");
    }
    debug_log("full path = \"%s\"\n", full_path);
    
    for (n = 0; n < max_patterns; ++n) {
        
        debug_log("Pattern %lu: press key to continue\n", n);
        #ifdef DEBUG_LOG
        getchar();
        #endif /* DEBUG_LOG */
        
        /* Load the next (n-th) initial pattern */
        pattern = malloc(max_units * sizeof (spike_T));
        assert(hn_read_next_pattern(pattern, full_path, max_units)
               != IO_FAILURE);
        
        /* Generate network structure with weights and the extracted pattern */
        net = hn_network_from_params(weights, threshold, pattern);
        
        /* Copy the array for later comparison */
        pcopy = hn_pattern_copy(pattern, max_units);
        
        /* Initialise network with parameters and use sequential mode */
        utils = hn_utils_with_mode(RANDOM);
        
        /* Determine the stable pattern, using the initial pattern
         * stored in net. A Pointer to net.activations is returned hence
         * we don't really need to use it */ 
        hn_test_pattern(net, NULL, max_units, warning_threshold, utils);
        
        /* Count the absolute frequency of matches between stable
         * and initial pattern */
        assert(net.activations != NULL);
        overlap = hn_overlap_frequency(net.activations, pcopy, max_units);
        
        printf("Pattern %lu: #overlaps = %lu\n", n, overlap);
        
        free(net.activations);
        net.activations = NULL;
        free(pcopy);
        pcopy = NULL;
        
    }
    
    /* Cleanup */
    hn_free(net);
    exit(EXIT_SUCCESS);
}
