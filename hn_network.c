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


#include <stdio.h>
#include <stdlib.h>

#include "../debug_log/debug_log.h"
#include "hn_macro_utils.h"
#include "hn_network.h"
#include "hn_types.h"


/* The following is only needed to visualize activation arrays for debugging */
#ifdef DEBUG_LOG

void print_arr(spike_T *arr, size_t length)
{
    for (int i = 0; i < length; ++i) {
        printf("%+d ", arr[i]);
    }
    printf("\n");
}

#else

#define print_arr(arr, length)  /* JUST BLANK */

#endif  /* DEBUG_LOG */



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
static int hn_update(size_t update_index, hn_network net, size_t max_units)
{
    /* Back up the current activation for the eventual comparison */
    spike_T current_activation = net.activations[update_index];
    
    /* Compute the local field */
    double local_field = 0.;    
    for (size_t i = 0; i < max_units; ++i) {
        local_field += net.weights[update_index][i] * net.activations[i];
    }
    
    /* Apply the activation function and update the unit */
    spike_T new_activation = Sign(local_field - net.threshold);
    net.activations[update_index] = new_activation;
    
    return current_activation != new_activation;
}


/* Compact-structure filler */
hn_network hn_network_from_params(double **weights, double threshold,
                                  spike_T *initial_pattern)
{
    hn_network network;
    network.weights = weights;
    network.threshold = threshold;
    network.activations = initial_pattern;
    Logger("Network data-structure successfully created\n");
    return network;
}


/* The hn_network structures are intended to be automatic.
 * This function is for internal array cleanup */
void hn_free(hn_network network)
{
    Logger("Freeing weights...\n");
    free(network.weights);
    network.weights = NULL;
    Logger("done!\n");
    Logger("Freeing activations...\n");
    free(network.activations);
    network.activations = NULL;
    Logger("done!\n");
}


size_t hn_overlap_frequency(spike_T *p1, spike_T *p2, size_t max_units)
{
    size_t overlaps = 0;
    for (size_t i = 0; i < max_units; ++i) {
        overlaps += p1[i] == p2[i];
    }
    return overlaps;
}


long hn_test_pattern(hn_network net, spike_T *pattern, size_t max_units,
                     size_t warning_threshold, hn_mode_utils utils)
{
    long update_counter = 0;
    /* At least one initial pattern must be present */
    KillUnless(pattern != NULL || net.activations != NULL);
    /* If pattern is specified, it overrides the network activations */
    if (net.activations == NULL) {
        Logger("net.activations == NULL\n");
        net.activations = pattern;
    }
    /* Resetting the selector before analysing a new pattern
     * (mandatory with the sequential selector) */
    utils.select_unit(max_units, 1);
    
    /* Repeat updates until the hard stability check tells to stop */
    Logger("Initiating main-test loop...\n");
    while (!utils.stability_check(net, max_units)) {
        /* Heuristic loose (but quicker) stability check performed inside */
	int unit_has_flipped;
        do {
            Logger("Current array:\n");
            print_arr(net.activations, max_units);
            
            size_t index_to_update = utils.select_unit(max_units, 0);
            
            Logger("Index to update: %lu\n", index_to_update);
            
            unit_has_flipped = hn_update(index_to_update, net, max_units);
            ++update_counter;
            
            Logger("Has unit flipped? %s\n\n",
                      unit_has_flipped ? "Yes" : "No");
            
        } while (!utils.stability_warning(unit_has_flipped, warning_threshold));
    }
    Logger("Exiting main-test loop\n\n");
    Logger("Final array:\n");
    print_arr(net.activations, max_units);
    
    return update_counter;
}


spike_T *hn_pattern_copy(spike_T *pattern, size_t max_units)
{
    spike_T *pattern_copy = malloc(max_units * sizeof (spike_T));
    KillUnless(pattern_copy != NULL);
    /* NULL if memcpy fails */
    return memcpy(pattern_copy, pattern, max_units * sizeof(*pattern));
}


void hn_hebb_weights_from_patterns(double **weights, spike_T **patterns,
                                int max_patterns, int max_units,
                                int remove_self_coupling)
{
    for (size_t i = 0; i < max_units; ++i) {
        for (size_t j = 0; j < max_units; ++j) {
            /* Initialise weights at 0 and, for each pattern,
             * add its autocorrelation to the weight matrix */
            weights[i][j] = 0.;
            for (size_t n = 0; n < max_patterns; ++n) {
                weights[i][j] += patterns[n][i] * patterns[n][j];
            }
            /* Normalise on number of units */
            weights[i][j] /= max_units;
        }
    }
    if (remove_self_coupling) {
        Logger("Weights: removing self-coupling\n");
        for (size_t i = 0; i < max_units; ++i) {
            weights[i][i] = 0.;
        }
    } else {
        Logger("Weights: keeping self-coupling\n");
    }
}


void hn_saturated_weights_from_patterns(double **weights, spike_T **patterns,
                                        double saturation, int max_patterns,
                                        int max_units, int remove_self_coupling)
{
    for (size_t i = 0; i < max_units; ++i) {
        for (size_t j = 0; j < max_units; ++j) {
            /* Initialise weights at 0 */
            weights[i][j] = 0.;
            for (size_t n = 0; n < max_patterns; ++n) {
                /* for each pattern, add its (normalised) autocorrelation */
                weights[i][j] += patterns[n][i] * patterns[n][j] / (double)max_units;
                /* Saturate excessive values */
                weights[i][j] = Min(weights[i][j], saturation);
                weights[i][j] = Max(weights[i][j], -saturation);
            }
        }
    }   
    if (remove_self_coupling) {
        Logger("Weights: removing self-coupling\n");
        for (size_t i = 0; i < max_units; ++i) {
            weights[i][i] = 0.;
        }
    } else {
        Logger("Weights: keeping self-coupling\n");
    }
}


void hn_hebb_weights_increment_with_pattern(double **weights,
                                            spike_T *pattern, int max_units,
                                            int remove_self_coupling)
{
    for (size_t i = 0; i < max_units; ++i) {
        for (size_t j = 0; j < max_units; ++j) {
            /* Add (normalised) pattern autocorrelation to the weight matrix */
            weights[i][j] += pattern[i] * pattern[j] / (double)max_units;
        }
    }   
    if (remove_self_coupling) {
        Logger("Weights: removing self-coupling\n");
        for (size_t i = 0; i < max_units; ++i) {
            weights[i][i] = 0.;
        }
    } else {
        Logger("Weights: keeping self-coupling\n");
    }
}


void hn_saturated_weights_increment_with_pattern(double **weights,
                                                 spike_T *pattern,
                                                 double saturation,
                                                 int max_units,
                                                 int inhibit_self_coupling)
{
    KillUnless(saturation > 0);
    
    for (size_t i = 0; i < max_units; ++i) {
        for (size_t j = 0; j < max_units; ++j) {
            /* Add (normalised) pattern autocorrelation to the weight matrix */
            weights[i][j] += pattern[i] * pattern[j] / (double)max_units;
            weights[i][j] = Min(weights[i][j], saturation);
            weights[i][j] = Max(weights[i][j], -saturation);
        }
    }
    
    if (inhibit_self_coupling) {
        Logger("Weights: removing self-coupling contribution\n");
        for (size_t i = 0; i < max_units; ++i) {
            weights[i][i] = 0.;
        }
    } else {
        Logger("Weights: allowing self-coupling\n");
    }
}
