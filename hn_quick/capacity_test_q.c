/*****************************************************
 * C FILE (main): capacity_test_q.c                  *
 * MODULE: Experimental capacity analysis            *
 *         with varying coding levels (proportion    *
 *         of selective neurons for patterns         *
 *         and activation function threshold)        *
 *                                                   *
 * PROJECT NAME: Hopfield Network simulation         *
 *               (prototype)                         *
 *                                                   *
 * SUPERVISOR: Mark van Rossum                       *
 *                                                   *
 * AUTHOR:  Lorenzo Mella                            *
 * VERSION: 31/07/2016                               *
 *                                                   *
 *****************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "hn_types.h"
#include "hn_macro_utils.h"
#include "hn_data_io.h"
#include "hn_modes_q.h" /* This replaces hn_modes */
#include "hn_network.h"
#include "hn_test_pattern_q.h" /* This replaces hn_test_pattern in hn_network */
/*#define DEBUG_LOG*/
#include "debug_log.h"


/* Application defaults are set here */
#define DEFAULT_MAX_TRIALS 10
#define DEFAULT_MAX_UNITS 500
#define DEFAULT_MAX_PATTERNS 125
#define DEFAULT_THRESHOLD 0.0
#define DEFAULT_CODING_LEVEL 0.5

#define SUPPRESS_SELF_COUPLING 1

#define SAVE_FOLDER "../avg_overlaps"

/* Little command-line parser */
void command_line_parser(int argc, char **argv, int *max_trials,
                         size_t *max_units, size_t *max_patterns,
                         double *threshold, double *coding_level);


int main(int argc, char **argv)
{
    /* Indices */
    size_t i;
    int trial;
    
    /* Command-line simulation parameters */
    int max_trials;         /* Number of MC simulation trials */
    size_t max_units;       /* Number of neurons */
    size_t max_patterns;    /* Maximum number of stored patterns */
    double threshold;       /* Activation function threshold */
    double coding_level;    /* Average proportion of +1s in patterns */
    
    /* Variables for timing */
    clock_t clock_start, clock_end; /* To time each trial */
    double secs_diff, total_elapsed_secs = 0.; /* To time the whole loop */
    
    /* Data structure pointers */
    spike_T **patterns = NULL;
    double **weights = NULL;
    double *avg_overlaps = NULL;
    
    /* String to hold customised savefile name */
    char s_filename[100];
    
    /* Seed if DEBUG_LOG is not toggled */
    #ifndef DEBUG_LOG
    srand(time(NULL));
    #endif
    
    command_line_parser(argc, argv, &max_trials, &max_units, &max_patterns,
                        &threshold, &coding_level);
    
    /* Program description to the user */
    printf("\n- Hopfield Network -\nRetrieval Probability estimation "
           "with random data generation\n\n");
    
    printf("MC estimate over %d trials.\n"
           "Number of units: %lu\tMemorised patterns: 1 to %lu\n"
           "Activation threshold: %g\n"
           "Coding level: %g\n\n", max_trials, max_units, max_patterns,
           threshold, coding_level);
    
    /* For each number of patterns we build the weights with those, then perform
     * an experiment on ONE pattern (among them, at random), then add
     * the result to the following. This is repeated max_trials times */
    avg_overlaps = malloc(max_patterns * sizeof (double));
    exit_on_exception(avg_overlaps != NULL);
    for (i = 0; i < max_patterns; ++i) {
        avg_overlaps[i] = 0.;
    }
    
    /* Main loop: identical experiments with randomised data
     * for Monte Carlo estimation of the retrieval probabilities */
    
    
    for (trial = 0; trial < max_trials; ++trial) {
        /* Timing each trial... */
        clock_start = clock();
        
        printf("trial %d start\n", trial+1);  /* A sort of progress bar */
        
        /* Allocate the space for a sequence of patterns and fill
         * patterns with random sequences at the specified coding level */
        /* THIS DOESN'T HOWEVER CHECK FOR IDENTICAL MEMORIES!! */
        debug_log("Allocating memory for patterns...\n");
        hn_matrix_alloc(patterns, max_patterns, max_units);
        debug_log("... done!\n");
        for (i = 0; i < max_patterns; ++i) {
            hn_fill_rand_pattern(patterns[i], coding_level, max_units);
        }
        
        /* Create weight matrix and set entries to 0.0 */
        debug_log("Creating a zero matrix for weights...\n");
        hn_matrix_zeros(weights, max_units, max_units);
        debug_log("... done!\n");
        
        /* Secondary loop: overlap frequency vs number of stored memories */
        for (i = 0; i < max_patterns; ++i) {
            hn_network net;
            hn_mode_utils_q utils = hn_utils_with_mode_q(RANDOM);
            
            /* Select a pattern among the first i+1 to test at random
             * and make a copy to compare later */
            spike_T *rand_pattern = patterns[RANDI(i+1)];
            spike_T *initial_state = hn_pattern_copy(rand_pattern, max_units);
            
            /* Update the weight matrix, learning the i-th pattern
             * incrementally (the 1 means diagonal is suppressed) */
            debug_log("Updating weights, learning pattern %lu...\n", i);
            hn_hebb_weights_increment_with_pattern(weights, patterns[i],
                                            max_units, SUPPRESS_SELF_COUPLING);
            debug_log("... done!\n");
            
            /* Build network and perform simulation on rand_pattern */
            net = hn_network_from_params(weights, threshold, NULL);
            debug_log("Testing rand_pattern...\n");
            hn_test_pattern_q(net, rand_pattern, max_units, max_units, utils);
            debug_log("... done!\n");
            /* (At this point rand_pattern has changed to a stable state) */
            
            /* Add the overlaps to the total counter (which will later
             * be turned into an average dividing by max_trials) */
            avg_overlaps[i] += hn_overlap_frequency(initial_state,
                                                    rand_pattern, max_units);
            
            free(initial_state);
        }
        debug_log("Freeing patterns and weights...\n");
        hn_matrix_free(weights);
        hn_matrix_free(patterns);
        debug_log("... done!\n");

        clock_end = clock();
        secs_diff = (double)(clock_end - clock_start)/CLOCKS_PER_SEC;
        total_elapsed_secs += secs_diff;
        
        printf("trial %d done. Elapsed CPU time: %.2f sec\n",
               trial+1, secs_diff);
    }
    
    printf("\nMain loop completed! Elapsed CPU time: %.2f sec\n\n",
           total_elapsed_secs);
    
    /* Making averages out of the accumulated results */
    for (i = 0; i < max_patterns; ++i) {
        avg_overlaps[i] /= max_trials;
    }
    
    /* Save average overlaps on a file (in folder hopfield/avg_overlaps) */
    sprintf(s_filename, "%s/avg_overlaps_q_%d_%lu_%lu_th%g_f%1.g.bin",
            SAVE_FOLDER, max_trials, max_units, max_patterns, threshold,
            coding_level);
    
    printf("Saving average overlap counts on file... ");
    exit_on_exception(IO_FAILURE != hn_save(avg_overlaps, s_filename,
                                            max_patterns));
    printf("done!\n\n");
    
    exit(EXIT_SUCCESS);
}


void command_line_parser(int argc, char **argv, int *max_trials,
                         size_t *max_units, size_t *max_patterns,
                         double *threshold, double *coding_level)
{
    /* Set defaults */
    *max_trials = DEFAULT_MAX_TRIALS;
    *max_units = DEFAULT_MAX_UNITS;
    *max_patterns = DEFAULT_MAX_PATTERNS;
    *threshold = DEFAULT_THRESHOLD;
    *coding_level = DEFAULT_CODING_LEVEL;
    
    /* Replace defaults in order if required (notice that failure
     * in strtod and strtol yields 0.0 and 0L values respectively) */
    switch (argc) {
        default: /* Ignore args beyond argv[5] */
        case 6:
            *coding_level = strtod(argv[5], NULL);
        case 5:
            *threshold = strtod(argv[4], NULL);
        case 4:
            *max_patterns = (size_t)strtol(argv[3], NULL, 10);
        case 3:
            *max_units = (size_t)strtol(argv[2], NULL, 10);
        case 2:
            *max_trials = (int)strtol(argv[1], NULL, 10);
        case 1:
            break;
    }
}
