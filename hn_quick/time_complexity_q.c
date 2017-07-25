/*****************************************************
 * C FILE (main): time_complexity_q.c                *
 * MODULE: Compute the time complexity of            *
 *         convergence both in number of             *
 *         elementary operations and number of       *
 *         unit updates                              *
 *                                                   *
 * PROJECT NAME: Hopfield Network simulation         *
 *               (prototype)                         *
 *                                                   *
 * SUPERVISOR: Mark van Rossum                       *
 *                                                   *
 * AUTHOR:  Lorenzo Mella                            *
 * VERSION: 28/07/2016                               *
 *                                                   *
 *****************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "hn_types.h"
#include "hn_macro_utils.h"
#include "hn_data_io.h"
#include "hn_modes_q.h" /* This replaces hn_modes */
#include "hn_network.h"
#include "hn_test_pattern_q.h" /* This replaces hn_test_pattern in hn_network */

#define DEBUG_LOG
#include "debug_log.h"

#define NEAREST_INTEGER(a)  (long)floor((a) + 0.5)

/* Application defaults are set here */
#define MAX_TRIALS 40
#define MAX_PLOT_VALUE 2000
#define MIN_PLOT_VALUE 100
#define MAX_PLOT_POINTS 10
#define PATTERN_UNIT_RATIO 0.05
#define CODING_LEVEL 0.5

#define SAVE_FOLDER "../time_complexity_plot"

enum self_coupling {KEEP_SELF_COUPLING=0, REMOVE_SELF_COUPLING=1};

/* Little command-line parser */
void command_line_parser(int argc, char **argv, int *max_trials,
                         size_t *max_plot_value, int *max_plot_points,
                         double *pattern_unit_ratio, double *coding_level);


int main(int argc, char **argv)
{
    /* Indices */
    size_t i, m;
    int trial;
    
    /* Parameters */
    int max_trials;
    size_t max_plot_value;
    size_t min_plot_value = MIN_PLOT_VALUE; /* We don't personalise this */
    int max_plot_points;
    double pattern_unit_ratio;
    double coding_level;
    
    /* Variables for timing (CPU time and number of updates) */
    clock_t clock_start, clock_end;
    double secs_diff;
    size_t timesteps;
    /* The following are the arrays that will be saved */
    double *avg_elapsed_secs;
    double *avg_timesteps;
    
    /* Save-file names */
    char savefile_points[100];
    char savefile_secs[100];
    char savefile_steps[100];
    
    /* Data structures to be filled at random etc. */
    size_t *plot_points;    /* Points in log-scale between min and max_units */
    double *dplot_points;   /* Copies to save (hn_save takes only doubles) */
    double ratio;           /* The ratio between consecutive plot points */
    hn_network net;
    double **weights;
    spike_T **patterns;
    hn_mode_utils_q utils = hn_utils_with_mode_q(RANDOM);
    
    /* Seed if DEBUG_LOG is not toggled */
    #ifndef DEBUG_LOG
    srand(time(NULL));
    #endif
    
    /* Setting parameters */
    command_line_parser(argc, argv, &max_trials, &max_plot_value,
                        &max_plot_points, &pattern_unit_ratio, &coding_level);
    
    /* Program description to the user */
    printf("\n- Hopfield Network -\nConvergence time estimation "
           "with random data generation\n\n");
    
    printf("Testing networks with %d different numbers of units (log scale)\n"
           "Units from %lu to %lu\nMemorised patterns: %.2f%% of the number "
           "of units)\nMC estimate over %d trials\nCoding level: %g\n\n",
           max_plot_points, min_plot_value, max_plot_value, pattern_unit_ratio,
           max_trials, coding_level);
    
    /* Now that we have the dimensions we can allocate time recording
     * data-structures. These must also be initialised at 0. */
    avg_elapsed_secs = malloc(max_plot_points * sizeof (double));
    exit_on_exception(avg_elapsed_secs != NULL);
    avg_timesteps = malloc(max_plot_points * sizeof (double));
    exit_on_exception(avg_timesteps != NULL);
    for (i = 0; i < max_plot_points; ++i) {
        avg_elapsed_secs[i] = 0.;
        avg_timesteps[i] = 0.;
    }
    
    /* Design the unique log_plot scale with size_t unit values
     * min_units <= num_units <= max_units and max_plot_points elements */
    plot_points = malloc(max_plot_points * sizeof (*plot_points));
    exit_on_exception(plot_points != NULL);
    dplot_points = malloc(max_plot_points * sizeof (*plot_points));
    exit_on_exception(dplot_points != NULL);
    
    ratio = pow(max_plot_value/(double)min_plot_value, 1./(max_plot_points-1));
    
    plot_points[0] = min_plot_value;
    dplot_points[0] = (double)min_plot_value;
    for (i = 1; i < max_plot_points; ++i) {
        /* next plot point = previous * ratio (rounded to nearest integer) */
        plot_points[i] = (size_t)NEAREST_INTEGER(plot_points[i-1] * ratio);
        debug_log("Nearest integer to %f is %lu\n", plot_points[i-1] * ratio,
                  plot_points[i]);
        dplot_points[i] = (double)plot_points[i];
    }
    
    /* Main loop for each number of units */
    for (i = 0; i < max_plot_points; ++i) {
        /* Allocation of data-structures for the present number of units (must
         * be done here because max_units and max_patterns change with i) */
        size_t max_units = plot_points[i];
        size_t max_patterns =
            (size_t)NEAREST_INTEGER(pattern_unit_ratio * max_units);
        debug_log("max_patterns = %lu\n", max_patterns);
        assert(max_patterns > 0 && max_patterns < max_units);
        
        hn_matrix_alloc(weights, max_units, max_units);
        hn_matrix_alloc(patterns, max_patterns, max_units);
        
        printf("Testing networks with %lu units\n", max_units);
        
        /* Secondary loop for MC estimation of times, give a number of units */
        for (trial = 0; trial < max_trials; ++trial) {
            spike_T *random_initial_state;
            printf("trial %d start...\n", trial+1);
            
            /* Create an initial pattern at random */
            random_initial_state = malloc(max_units * sizeof (spike_T));
            exit_on_exception(random_initial_state != NULL);
            hn_fill_rand_pattern(random_initial_state, coding_level, max_units);
            
            /* Create a number of patterns max_patterns (can be constrained
             * as a function of max_units) and generate weights from them */
            for (m = 0; m < max_patterns; ++m) {
                hn_fill_rand_pattern(patterns[m], coding_level, max_units);
            }
            hn_hebb_weights_from_patterns(weights, patterns, max_patterns,
                                          max_units, REMOVE_SELF_COUPLING);
            
            /* Create network package */
            net.weights = weights;
            net.activations = random_initial_state;
            
            /* Test the pattern (and accumulate the number of iterations) */
            clock_start = clock();
            timesteps = hn_test_pattern_q(net, NULL, max_units, max_units,
                                          utils);
            clock_end = clock();
            avg_timesteps[i] += (double)timesteps;
            
            /* CPU timing operations */
            secs_diff = (double)(clock_end - clock_start)/CLOCKS_PER_SEC;
            avg_elapsed_secs[i] += secs_diff;
            printf("trial %d complete. Elapsed CPU time: %.2f secs\n"
                   "Number of updates: %lu\n", trial+1, secs_diff, timesteps);
            
            /* Secondary loop cleanup */
            free(random_initial_state);
        }
        
        /* From totals to averages over trials */
        avg_elapsed_secs[i] /= max_trials;
        avg_timesteps[i] /= max_trials;
        
        printf("Number of units: %lu\nAvg elapsed time over trials: %.2f\n"
               "Avg number of timesteps over trials: %.2f\n\n", plot_points[i],
               avg_elapsed_secs[i], avg_timesteps[i]);
        
        /* Main loop cleanup */
        hn_matrix_free(patterns);
        hn_matrix_free(weights);
    }
    
    printf("All numbers of units have been tested\n\n");
    
    /* Create filenames with meaningful parameter information */
    sprintf(savefile_points, "%s/tc_plot_points_q_%d_%.3f.bin", SAVE_FOLDER,
            max_trials, pattern_unit_ratio);
    sprintf(savefile_secs, "%s/tc_plot_secs_q_%d_%.3f.bin", SAVE_FOLDER,
            max_trials, pattern_unit_ratio);
    sprintf(savefile_steps, "%s/tc_plot_steps_q_%d_%.3f.bin", SAVE_FOLDER,
            max_trials, pattern_unit_ratio);
    
    /* Save data (dplot_points, avg_elapsed_secs and avg_timesteps) on files */
    printf("Saving files in ./%s\n", SAVE_FOLDER);
    printf("Saving list of numbers of units on file: %s\n", savefile_points);
    exit_on_exception(IO_SUCCESS ==
                      hn_save(dplot_points, savefile_points, max_plot_points));
    printf("...done!\n");
    printf("Saving times (in seconds) on file: %s\n", savefile_secs);
    exit_on_exception(IO_SUCCESS ==
                     hn_save(avg_elapsed_secs, savefile_secs, max_plot_points));
    printf("...done!\n");
    printf("Saving numbers of updates on file: %s\n", savefile_steps);
    exit_on_exception(IO_SUCCESS ==
                      hn_save(avg_timesteps, savefile_steps, max_plot_points));
    printf("...done!\n");
    
    /* Cleanup */
    free(avg_timesteps);
    free(avg_elapsed_secs);
    free(dplot_points);
    free(plot_points);
    
    exit(EXIT_SUCCESS);
}


void command_line_parser(int argc, char **argv, int *max_trials,
                         size_t *max_plot_value, int *max_plot_points,
                         double *pattern_unit_ratio, double *coding_level)
{
    /* Set defaults */
    *max_trials = MAX_TRIALS;
    *max_plot_value = MAX_PLOT_VALUE;
    *max_plot_points = MAX_PLOT_POINTS;
    *pattern_unit_ratio = PATTERN_UNIT_RATIO;
    *coding_level = CODING_LEVEL;
    
    /* Replace defaults in order if required (notice that failure
     * in strtod and strtol yields 0.0 and 0L values respectively) */
    switch (argc) {
        default: /* Ignore args beyond argv[4] */
        case 6:
            *coding_level = strtod(argv[5], NULL);
        case 5:
            *pattern_unit_ratio = strtod(argv[4], NULL);
        case 4:
            *max_plot_points = (int)strtol(argv[3], NULL, 10);
        case 3:
            *max_plot_value = (size_t)strtol(argv[2], NULL, 10);
        case 2:
            *max_trials = (int)strtol(argv[1], NULL, 10);
        case 1:
            break;
    }
}
