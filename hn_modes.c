/*****************************************************
 * HEADER FILE: hn_modes.c                           *
 * MODULE: Utility functions depending on            *
 *         the update mode                           *
 *                                                   *
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
#include "hn_types.h"
#include "hn_macro_utils.h"
#include "hn_modes.h"
#include "../debug_log/debug_log.h"


hn_mode_utils hn_utils_with_mode(char *update_mode)
{
    hn_mode_utils utils;
    if (StringsAreEqual(update_mode, "Sequential")) {
	utils.select_unit = &sequential_select_unit;
	utils.stability_warning = &sequential_stability_warning;
	utils.stability_check = &sequential_stability_check;
    } else {
	if (!StringsAreEqual(update_mode, "Random")) {
	    fprintf(stderr, "%s: Unknown update mode %s."
		    "Defaulting to Random\n", __func__, update_mode);
	}
	utils.select_unit = &random_select_unit;
	utils.stability_warning = &sequential_stability_warning;
	utils.stability_check = &random_stability_check;
    }
    return utils;
}


size_t sequential_select_unit(size_t max_units, int reset)
{
    static size_t counter = 0;
    if (reset) {
        return (counter = 0);
    } else {
        return counter++ % max_units;
    }
}


int sequential_stability_warning(int unit_has_flipped, size_t threshold)
{
    /* Count how many consecutive updates don't result in changes */
    static size_t stability_counter = 0;

    if (unit_has_flipped) {
        stability_counter = 0;
    } else {
        ++stability_counter;
    }
    
    Logger("stability_counter = %lu\n", stability_counter);
    
    /* As soon as we have a warning, reset the counter for future use
     * and return 1, otherwise return 0 */
    if (stability_counter >= threshold) {
        stability_counter = 0;
        return 1;
    } else {
        return 0;
    }
}


int sequential_stability_check(hn_network net, size_t max_units)
{
    size_t i, j;
    double local_field;
    
    for (i = 0; i < max_units; ++i) {
        /* Compute the local field */
        local_field = 0.;
        for (j = 0; j < max_units; ++j) {
            local_field += net.weights[i][j] * net.activations[j];
        }
        
        /* Check if the unit is stable, i.e., activation doesn't change */
        if (Sign(local_field - net.threshold) != net.activations[i]) {
            return 0;
        }
    }
    
    Logger("Last verified index checked (Sequential mode): %lu\n", i);
    
    return 1;
}


size_t random_select_unit(size_t max_units, int reset)
{
    return RandI(max_units);
}


int random_stability_check(hn_network net, size_t max_units)
{
    size_t i;    
    for (i = 0; i < max_units; ++i) {
        /* Compute the local field */
        double local_field = 0.;
        for (size_t j = 0; j < max_units; ++j) {
            local_field += net.weights[i][j] * net.activations[j];
        }
        /* Check if the unit is stable, i.e., activation doesn't change */
        if (Sign(local_field - net.threshold) != net.activations[i]) {
            return 0;
        }
    }
    Logger("Last verified index checked (RANDOM mode): %lu\n", i);
    return 1;
}
