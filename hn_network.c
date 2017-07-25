/*****************************************************
 * C FILE: hn_network.c                              *
 * MODULE: Main simulation                           *
 *                                                   *
 * PROJECT NAME: Hopfield Network simulation         *
 *               (prototype)                         *
 *                                                   *
 * SUPERVISOR: Mark van Rossum                       *
 *                                                   *
 * AUTHOR:  Lorenzo Mella                            *
 * VERSION: 23/07/2016                               *
 *                                                   *
 *****************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "hn_types.h"
#include "hn_macro_utils.h"
#include "hn_network.h"
#include "debug_log.h"


/* The following is only needed to visualise
 * (short!) activation arrays for debugging */
#ifdef DEBUG_LOG
void debug_print_arr(spike_T *arr, size_t length);
#else
#define debug_print_arr(arr, length) /* JUST BLANK */
#endif /* DEBUG_LOG */


/**
 * Perform a single update (hence asynchronously) of specific unit
 * given the current state of the network.
 *
 * @param update_index: the index of the unit to be updated
 * @param network:      the Hopfield Network data structure
 * @param max_units:    the size of the network
 *
 * @return:             1 if the activation has changed, 0 otherwise
 */
static int hn_update(size_t update_index, hn_network network, size_t max_units);


/* These two functions are pretty useless but the concept of having
 * constructor and destructor survived from the previous implementation.
 * At least they are legible... but it is fairly unconventional for
 * a "constructor" to return a structure and not a pointer to it... */
hn_network hn_network_from_params(double **weights, double threshold,
                                  spike_T *initial_pattern)
{
    hn_network network;
    network.weights = weights;
    network.threshold = threshold;
    network.activations = initial_pattern;
    debug_log("Network data-structure successfully created\n");
    return network;
}

void hn_free(hn_network network)
{
    debug_log("Freeing weights...\n");
    free(network.weights);
    network.weights = NULL;
    debug_log("done!\n");
    debug_log("Freeing activations...\n");
    free(network.activations);
    network.activations = NULL;
    debug_log("done!\n");
}


size_t hn_overlap_frequency(spike_T *p1, spike_T *p2, size_t max_units)
{
    size_t i;
    size_t overlaps = 0;
    for (i = 0; i < max_units; ++i) {
        overlaps += (p1[i] == p2[i]);
    }
    return overlaps;
}

long hn_test_pattern(hn_network net, spike_T *pattern, size_t max_units,
                     size_t warning_threshold, hn_mode_utils utils)
{
    size_t index_to_update;
    long update_counter = 0;
    int unit_has_flipped;
    
    /* At least one initial pattern must be present */
    assert(pattern != NULL || net.activations != NULL);
    /* If pattern is specified, it overrides the network activations */
    if (net.activations == NULL) {
        debug_log("net.activations == NULL\n");
        net.activations = pattern;
    }
    /* Resetting the selector before analysing a new pattern
     * (necessary with the sequential selector) */
    utils.select_unit(max_units, 1);
    
    /* Repeat updates until the hard stability check tells to stop */
    debug_log("Initiating main-test loop...\n");
    while (!utils.stability_check(net, max_units)) {
        /* Heuristic loose (but quicker) stability check performed inside */
        do {
            debug_log("Current array:\n");
            debug_print_arr(net.activations, max_units);
            
            index_to_update = utils.select_unit(max_units, 0);
            
            debug_log("Index to update: %lu\n", index_to_update);
            
            unit_has_flipped = hn_update(index_to_update, net, max_units);
            ++update_counter;
            
            debug_log("Has unit flipped? %s\n\n",
                      unit_has_flipped ? "Yes" : "No");
            
        } while (!utils.stability_warning(unit_has_flipped, warning_threshold));
    }
    debug_log("Exiting main-test loop\n\n");
    debug_log("Final array:\n");
    debug_print_arr(net.activations, max_units);
    
    return update_counter;
}


spike_T *hn_pattern_copy(spike_T *pattern, size_t max_units)
{
    size_t i;
    
    spike_T *pattern_copy = malloc(max_units * sizeof (spike_T));
    exit_on_exception(pattern_copy != NULL);
    
    for (i = 0; i < max_units; ++i) {
        pattern_copy[i] = pattern[i];
    }
    return pattern_copy;
}


static int hn_update(size_t update_index, hn_network net, size_t max_units)
{
    size_t i;
    /* Copying current activation for later comparison */
    spike_T current_activation = net.activations[update_index];
    spike_T new_activation;

    /* Compute the local field */
    double local_field = 0.;    
    for (i = 0; i < max_units; ++i) {
        local_field += net.weights[update_index][i] * net.activations[i];
    }
    
    /* Apply activation function and update unit */
    new_activation = SIGN(local_field - net.threshold);
    net.activations[update_index] = new_activation;
    
    return current_activation != new_activation;
}


void hn_hebb_weights_from_patterns(double **weights, spike_T **patterns,
                                int max_patterns, int max_units,
                                int remove_self_coupling)
{
    size_t i, j, n;
    
    for (i = 0; i < max_units; ++i) {
        for (j = 0; j < max_units; ++j) {
            /* Initialise weights at 0 and, for each pattern,
             * add its autocorrelation to the weight matrix */
            weights[i][j] = 0.;
            for (n = 0; n < max_patterns; ++n) {
                weights[i][j] += patterns[n][i]*patterns[n][j];
            }
            /* Normalise on number of units */
            weights[i][j] /= max_units;
        }
    }
    
    if (remove_self_coupling) {
        debug_log("Weights: removing self-coupling\n");
        for (i = 0; i < max_units; ++i) {
            weights[i][i] = 0.;
        }
    } else {
        debug_log("Weights: keeping self-coupling\n");
    }
}


void hn_saturated_weights_from_patterns(double **weights, spike_T **patterns,
                                        double saturation, int max_patterns,
                                        int max_units, int remove_self_coupling)
{
    size_t i, j, n;
    
    for (i = 0; i < max_units; ++i) {
        for (j = 0; j < max_units; ++j) {
            /* Initialise weights at 0 */
            weights[i][j] = 0.;
            for (n = 0; n < max_patterns; ++n) {
                /* for each pattern, add its (normalised) autocorrelation */
                weights[i][j] += patterns[n][i]*patterns[n][j] /
                                 (double)max_units;
                /* Saturate excessive values */
                weights[i][j] = MIN(weights[i][j], saturation);
                weights[i][j] = MAX(weights[i][j], -saturation);
            }
        }
    }
    
    if (remove_self_coupling) {
        debug_log("Weights: removing self-coupling\n");
        for (i = 0; i < max_units; ++i) {
            weights[i][i] = 0.;
        }
    } else {
        debug_log("Weights: keeping self-coupling\n");
    }
}


void hn_hebb_weights_increment_with_pattern(double **weights,
                                            spike_T *pattern, int max_units,
                                            int remove_self_coupling)
{
    size_t i, j;
    
    for (i = 0; i < max_units; ++i) {
        for (j = 0; j < max_units; ++j) {
            /* Add (normalised) pattern autocorrelation to the weight matrix */
            weights[i][j] += pattern[i]*pattern[j] / (double)max_units;
        }
    }
    
    if (remove_self_coupling) {
        debug_log("Weights: removing self-coupling\n");
        for (i = 0; i < max_units; ++i) {
            weights[i][i] = 0.;
        }
    } else {
        debug_log("Weights: keeping self-coupling\n");
    }
}


void hn_saturated_weights_increment_with_pattern(double **weights,
                                                 spike_T *pattern,
                                                 double saturation,
                                                 int max_units,
                                                 int remove_self_coupling)
{
    size_t i, j;
    
    assert(saturation > 0);
    
    for (i = 0; i < max_units; ++i) {
        for (j = 0; j < max_units; ++j) {
            /* Add (normalised) pattern autocorrelation to the weight matrix */
            weights[i][j] += pattern[i]*pattern[j] / (double)max_units;
            weights[i][j] = MIN(weights[i][j], saturation);
            weights[i][j] = MAX(weights[i][j], -saturation);
        }
    }
    
    if (remove_self_coupling) {
        debug_log("Weights: removing self-coupling\n");
        for (i = 0; i < max_units; ++i) {
            weights[i][i] = 0.;
        }
    } else {
        debug_log("Weights: keeping self-coupling\n");
    }
}


#ifdef DEBUG_LOG
void debug_print_arr(spike_T *arr, size_t length)
{
    int i;
    for (i = 0; i < length; ++i) {
        printf("%+d ", arr[i]);
    }
    putchar('\n');
}
#endif /* DEBUG_LOG */