#include <stdlib.h>
#include <assert.h>
#include "hn_types.h"
#include "hn_macro_utils.h"
#include "hn_network.h"
#include "hn_modes_q.h"
#include "../debug_log/debug_log.h"
#include "hn_test_pattern_q.h"


static void initialise_flipped_units(int *flipped_units, hn_network net,
                                     size_t max_units);

static int hn_update(size_t update_index, hn_network net, size_t max_units);

long hn_test_pattern_q(hn_network net, spike_T *pattern, size_t max_units,
                       size_t warning_threshold, hn_mode_utils_q utils)
{
    long update_counter = 0;
    int unit_has_flipped;
    int *flipped_units = malloc(max_units * sizeof (int));
    KillUnless(flipped_units != NULL);
    
    /* At least one initial pattern must be present */
    assert(pattern != NULL || net.activations != NULL);
    /* If pattern is specified, it overrides the network activations */
    if (net.activations == NULL) {
        net.activations = pattern;
    }
    
    /* Initialise flipped_units to the next stability value */
    /* initialise_flipped_units(flipped_units, net, max_units); */
    /* Initialise flipped_units at random */
    random_initialise_flipped_units(flipped_units, max_units);
    
    /* Resetting the selector before analysing a new pattern
     * (necessary with the sequential selector) */
    utils.select_unit(max_units, 1);
    
    /* Repeat updates until the hard stability check tells to stop */
    while (!utils.stability_check(flipped_units, max_units)) {
        /* Heuristic loose (but quicker) stability check performed inside */
        do {
            size_t index_to_update = utils.select_unit(max_units, 0);
            Logger("index_to_update: %lu\n", index_to_update);
            unit_has_flipped = hn_update(index_to_update, net, max_units);
            flipped_units[index_to_update] = unit_has_flipped;
            ++update_counter;
            Logger("flipped_units[index_to_update] = %d\n",
                      flipped_units[index_to_update]);
        } while (!utils.stability_warning(unit_has_flipped, warning_threshold));
    }
    
    /* cleanup */
    free(flipped_units);
    
    return update_counter;
}


static void initialise_flipped_units(int *flipped_units, hn_network net,
                                     size_t max_units)
{
    size_t i, j;
    double local_field;
    
    for (i = 0; i < max_units; ++i) {
        /* Compute the local field */
        local_field = 0.;
        for (j = 0; j < max_units; ++j) {
            local_field += net.weights[i][j] * net.activations[j];
        }
        
        /* flipped_units = 1 if activations change */
        flipped_units[i] = SIGN(local_field - net.threshold) !=
                           net.activations[i];
    }
}


static void random_initialise_flipped_units(int *flipped_units,
                                            size_t max_units)
{
    size_t i;
    for (i = 0; i < max_units; ++i) {
        /* flipped_units = 1 if activations change */
        flipped_units[i] = rand()/(double)RAND_MAX < 0.5;
    }
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
