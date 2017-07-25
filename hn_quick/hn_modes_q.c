/*****************************************************
 * HEADER FILE: hn_modes_q.c                         *
 * MODULE: Utility functions depending on            *
 *         the update mode  (to be used with         *
 *         hn_test_pattern_q)                        *
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
#include "hn_modes_q.h"
#include "debug_log.h"


hn_mode_utils_q hn_utils_with_mode_q(update_mode mode)
{
    hn_mode_utils_q utils;
    
    switch (mode) {
        case SEQUENTIAL:
            utils.select_unit = &sequential_select_unit_q;
            utils.stability_warning = &sequential_stability_warning_q;
            utils.stability_check = &sequential_stability_check_q;
            break;
        case RANDOM:
        default:
            utils.select_unit = &random_select_unit_q;
            utils.stability_warning = &sequential_stability_warning_q;
            utils.stability_check = &random_stability_check_q;
            break;
    }
    return utils;
}


size_t sequential_select_unit_q(size_t max_units, int reset)
{
    static size_t counter = 0;
    if (reset) {
        return (counter = 0);
    } else {
        return counter++ % max_units;
    }
}


int sequential_stability_warning_q(int unit_has_flipped, size_t threshold)
{
    /* Count how many consecutive updates don't result in changes */
    static size_t stability_counter = 0;

    if (unit_has_flipped) {
        stability_counter = 0;
    } else {
        ++stability_counter;
    }
    
    debug_log("stability_counter = %lu\n", stability_counter);
    
    /* As soon as we have a warning, reset the counter for future use
     * and return 1, otherwise return 0 */
    if (stability_counter >= threshold) {
        stability_counter = 0;
        return 1;
    } else {
        return 0;
    }
}


int sequential_stability_check_q(int *flipped_units, size_t max_units)
{
    size_t i;
    for (i = 0; i < max_units; ++i) {
        /* If even one unit has flipped, stability fails */
        if (flipped_units[i] != 0) {
            return 0;
        }
    }
    /* We get here only if all units are stable */
    return 1;
}


size_t random_select_unit_q(size_t max_units, int reset)
{
    return RANDI(max_units);
}


int random_stability_check_q(int *flipped_units, size_t max_units)
{
    size_t i;
    for (i = 0; i < max_units; ++i) {
        /* If even one unit has flipped, stability fails */
        if (flipped_units[i] != 0) {
            return 0;
        }
    }
    /* We get here only if all units are stable */
    return 1;
}
