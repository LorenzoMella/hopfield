/*****************************************************
 * HEADER FILE: hn_modes.h                           *
 * MODULE: Utility functions depending on            *
 *         the update mode                           *
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
 *                                                   *
 *****************************************************/

#ifndef HN_MODES_H
#define HN_MODES_H


#include "hn_types.h"

#include <stdlib.h>



/* There is no difference between the warning mechanisms,
 * but having a dedicated name is nice */
#define random_stability_warning    sequential_stability_warning


/**
 * Provide a 'package' with utility functions to be used in the simulation,
 * coherent with the specified update mode.
 * 
 * \param mode    the update mode
 * 
 * \return        a structure with pointers to the utility functions
 */
hn_mode_utils hn_utils_with_mode(enum hn_mode mode);


/**
 * At each call, generate the next index (wrap around after max_units).
 * 
 * \param max_units    the size of the network
 * \param reset        a reset signal: static counter is set to 0 iff reset = 1
 * 
 * \return             the index of the unit to update
 */
size_t sequential_select_unit(size_t max_units, int reset);


/**
 * Heuristic test for convergence: simply checks whether the number of
 * consecutive stable updates (no change in activation) has reached a
 * threshold.
 * 
 * \param unit_has_flipped    the size of the network
 * \param threshold           the size of the network
 * 
 * \return                    1 if warning is issued, 0 otherwise
 */
int sequential_stability_warning(int unit_has_flipped, size_t threshold);


/**
 * Checks whether all units are stable.
 * 
 * \param net          the Hopfield Network data-structure
 * \param max_units    the size of the network
 * 
 * \return             1 if the test is successful, 0 otherwise
 */
int sequential_stability_check(hn_network net, size_t max_units);


/**
 * At each call, generate the next index uniformly at random.
 * 
 * \param max_units    the size of the network
 * \param reset        (no effect)
 * 
 * \return             the index of the unit to update
 */
size_t random_select_unit(size_t max_units, int reset);


/**
 * Checks whether all units are stable (duplicate of
 * sequential_stability_check()).
 * 
 * \param net          the Hopfield Network data-structure
 * \param max_units    the size of the network
 * 
 * \return             1 if the test is successful, 0 otherwise
 */
int random_stability_check(hn_network, size_t max_units);


#endif  /* HN_MODES_H */
