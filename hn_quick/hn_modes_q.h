/*****************************************************
 * HEADER FILE: hn_modes_q.h                         *
 * MODULE: Utility functions depending on            *
 *         the update mode (to be used with          *
 *         hn_test_pattern_q)                        *
 *                                                   *
 * FUNCTION: Assigns a "function package"            *
 *           to the hn_mode_utils data-structure     *
 *           depending on  unit selection mode       *
 *           to be used in the main simulation       *
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

#ifndef HN_MODES_Q_H
#define HN_MODES_Q_H

#include <stdlib.h>
#include "hn_types.h"


/**
 * Provide a 'package' with utility functions to be used in the simulation,
 * coherent with the specified update mode.
 * 
 * @param mode:    the update mode
 * 
 * @return:        a structure with pointers to the utility functions
 */
hn_mode_utils_q hn_utils_with_mode_q(update_mode mode);


/**
 * At each call, generate the next index (wrap around after max_units).
 * 
 * @param max_units:    the size of the network
 * @param reset:        a reset signal: static counter is set to 0 iff reset = 1
 * 
 * @return:             the index of the unit to update
 */
size_t sequential_select_unit_q(size_t max_units, int reset);


/**
 * Heuristic test for convergence: simply checks whether the number of
 * consecutive stable updates (no change in activation) has reached a
 * threshold.
 * 
 * @param unit_has_flipped:    the size of the network
 * @param threshold:           the size of the network
 * 
 * @return:                    1 if warning is issued, 0 otherwise
 */
int sequential_stability_warning_q(int unit_has_flipped, size_t threshold);
/*
 * I'm not introducing the analogous
 *
 * int random_stability_warning(int unit_has_flipped, int threshold);
 *
 * because I'd code it exactly as sequential_stability_warning().
 */


/**
 * Checks whether all units are stable.
 * 
 * @param flipped_units:    array indicating which units are stable/unstable
 * @param max_units:        the size of the network
 * 
 * @return:                 1 if the test is successful, 0 otherwise
 */
int sequential_stability_check_q(int *flipped_units, size_t max_units);


/**
 * At each call, generate the next index uniformly at random.
 * 
 * @param max_units:    the size of the network
 * @param reset:        (no effect)
 * 
 * @return:             the index of the unit to update
 */
size_t random_select_unit_q(size_t max_units, int reset);


/**
 * Checks whether all units are stable (duplicate of
 * sequential_stability_check()).
 * 
 * @param net:          the Hopfield Network data-structure
 * @param max_units:    the size of the network
 * 
 * @return:             1 if the test is successful, 0 otherwise
 */
int random_stability_check_q(int *flipped_units, size_t max_units);

#endif
