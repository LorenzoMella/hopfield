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
 *                                                   *
 *****************************************************/


#include "debug_log.h"
#include "hn_types.h"
#include "hn_macro_utils.h"
#include "hn_modes.h"

#include <stdlib.h>
#include <stdio.h>


hn_mode_utils hn_utils_with_mode(enum hn_mode update_mode)
{
    hn_mode_utils utils;
    
    if (update_mode == MODE_SEQUENTIAL) {

	utils.select_unit = &sequential_select_unit;
	utils.stability_warning = &sequential_stability_warning;
	utils.stability_check = &sequential_stability_check;
        
    } else {  /* the only other option: MODE_RANDOM */
        
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
    size_t i;
    
    for (i = 0; i < max_units; ++i) {

        /* Compute the local field */
        double local_field = 0.;
        for (size_t j = 0; j < max_units; ++j) {
            local_field += net.weights[i][j] * net.activations[j];
        }
        
        /* Check if the unit is stable, i.e., the activation doesn't change */
        if (Sign(local_field - net.threshold) != net.activations[i]) {
            return 0;
        }
    }
    
    Logger("Last verified index (MODE_SEQUENTIAL): %lu\n", i);
    
    return 1;
}


size_t random_select_unit(size_t max_units, int reset)
{
    /* (The reset argument is unused) */
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
    
    Logger("Last verified index (MODE_RANDOM): %lu\n", i);

    return 1;
}
