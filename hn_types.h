/*****************************************************
 * HEADER FILE: hn_types.h                           *
 *                                                   *
 * FUNCTION: Definitions used throughout of          *
 *           data-structures, etc.                   *
 *                                                   *
 * PROJECT NAME: Hopfield Network simulation         *
 *               (prototype)                         *
 *                                                   *
 * SUPERVISOR: Mark van Rossum                       *
 *                                                   *
 * AUTHOR:  Lorenzo Mella                            *
 * VERSION: 27/06/2016                               *
 *                                                   *
 *****************************************************/

#ifndef HN_TYPES_H
#define HN_TYPES_H

#include <stdlib.h>


/**
 * This is the type of neuron activation: to be used only with values +1 and -1.
 * Maybe bad practice, but spikes may change representation in later versions.
 */
typedef int spike_T;


/**
 * Update mode: the way that a neuron is selected for update at each iteration.
 */
enum hn_mode {
    MODE_SEQUENTIAL,            /* Th next unit according to index order is selected */
    MODE_RANDOM                 /* A random unit is selected ("with replacement") */
};


/**
 * Container for the getopt arguments
 */
typedef struct hn_options {

    size_t max_units;       /* size of the network */
    size_t max_patterns;    /* number of patterns in datafile */
    char *w_filename;       /* name of datafile with network weights */
    char *p_filename;       /* name of datafile with initial sequences */
    char *s_filename;       /* output savefile name */
    enum hn_mode mode;      /* update mode */
    double threshold;       /* the activation function threshold */

} hn_options;


/**
 * A simple collection of the main parameters that describe the Network.
 * Used to pass the information more compactly.
 */ 
typedef struct hn_network {

    double **weights;       /* matrix of size max_units * max_units */
    spike_T *activations;   /* vector of length max_units */
    double threshold;       /* common threshold to all the units */

} hn_network;


/**
 * Collection of function pointers to utility functions to be used by
 * hn_test_pattern and take into account how the next unit is to be selected
 * for update.
 */
typedef struct hn_mode_utils {

    /* Generates the next index of the next unit to update. It could have
     * an internal state that can be reset with the boolean reset */
    size_t (*select_unit)(size_t max_units, int reset);

    /* Signals whether we should test for convergence, that is,
     * use stability_check() */
    int (*stability_warning)(int unit_has_flipped, size_t threshold);

    /* Determines whether the network has converged */
    int (*stability_check)(hn_network, size_t max_units);

} hn_mode_utils;


/**
 * Same as above, but with slightly different signatures: to be used
 * with the quicker hn_test_pattern_q.
*/
typedef struct hn_mode_utils_q {

    size_t (*select_unit)(size_t max_units, int reset);

    int (*stability_warning)(int unit_has_flipped, size_t threshold);

    int (*stability_check)(int *flipped_units, size_t max_units);

} hn_mode_utils_q;


#endif /* HN_TYPES_H */
