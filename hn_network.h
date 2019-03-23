/*****************************************************
 * HEADER FILE: hn_network.h                         *
 * MODULE: Main simulation                           *
 *                                                   *
 * FUNCTION: Creation of the HN data structure       *
 *           from preprocessed data and network      *
 *           update cycle                            *
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

#ifndef HN_NETWORK_H
#define HN_NETWORK_H


#include "hn_types.h"


/**
 * Create the network data-structure.
 *
 * @param weights:         max_units * max_units symmetric matrix;
 *                         row i provides the weights for the i-th unit
 * @param threshold:       the threshold of the activation function
 * @param initial_state:   initial stimulus for memory recall (max_units array)
 *
 * @return:                the structure representing the Hopfield Network
 */
hn_network hn_network_from_params(double **weights, double threshold,
                                  spike_T *initial_state);


/**
 * Free the allocated arrays in the Hopfield Network data-structure
 * (could be confusing. It's better to deallocate hn_network.weights and
 * hn_network.activations "manually").
 *
 * @param hn_network:  The Hopfield Network data structure
 */
void hn_free(hn_network network);


/**
 * Count the matches between the two provided patterns.
 *
 * @param p1:           first pattern
 * @param p2:           second pattern
 * @param max_units:    the size of the network (and patterns)
 *
 * @return:             the number of matches between the patterns
 */
size_t hn_overlap_frequency(spike_T *p1, spike_T *p2, size_t max_units);


/**
 * Simulate the system dynamics, through continuous asynchronous updates
 * of the state, stop at convergence and return the number of performed updates
 * (which may be zero, because the check for convergence is done at the outset
 * of the main loop).
 *
 * @param network:           the Hopfield Network data structure
 * @param pattern:           the initial pattern that we want to test
 * @param max_units:         the size of the network
 * @param warning_threshold: stable-unit counter threshold
 * @param utils:             functions to be used, depending on update mode
 *
 * @return:                  the number of unit updates until convergence
 */
long hn_test_pattern(hn_network net, spike_T *pattern, size_t max_units,
                     size_t warning_threshold, hn_mode_utils utils);


/**
 * Copy a pattern vector.
 * 
 * @param pattern:      the pattern to be copied
 * @param max_units:    the size of the network
 *
 * @return:             a pointer to the copy of the pattern (freed by user)
 */
spike_T *hn_pattern_copy(spike_T *pattern, size_t max_units);

/**
 * Create a weight matrix using Hebb's rule on a list of patterns;
 * the weight matrix is expected to be pre-allocated with
 * dimensions max_units * max_units.
 *
 * @param weights:              the weight matrix to be filled
 * @param patterns:             list of spike_T patterns
 * @param max_patterns:         the number of patterns
 * @param max_units:            the size of the network
 * @param remove_self_coupling  non-zero to suppress diagonal, 0 otherwise
 */
void hn_hebb_weights_from_patterns(double **weights, spike_T **pattern,
				   int max_patterns, int max_units,
				   int remove_self_coupling);


/**
 * Add the autocorrelation (outer product with itself) of pattern
 * to the matrix weights; the diagonal is suppressed iff remove_self_coupling
 * is non-zero; Incremental version of hn_hebb_weights_from_patterns().
 *
 * @param weights:              the weight matrix to be updated
 * @param pattern:              the pattern to be learnt
 * @param max_units:            the size of the network
 * @param remove_self_coupling  1 to suppress diagonal (else 0)
 */
void hn_hebb_weights_increment_with_pattern(double **weights,
                                            spike_T *pattern, int max_units,
                                            int remove_self_coupling);


/* THE TWO FOLLOWING FUNCTIONS ARE STRAIGHTFORWARD VARIANTS OF THE ABOVE,
 * BUT THEY HAVEN'T BEEN TESTED! */
 
/**
 * Create a weight matrix using Hebb's rule on a list of patterns;
 * the contribution of each autocorrelation (normalised with 1./max_units)
 * is added, then the value is saturated in [-saturation, saturation];
 * the weight matrix is expected to be pre-allocated with
 * dimensions max_units * max_units.
 *
 * @param weights:              the weight matrix to be filled
 * @param patterns:             list of spike_T patterns
 * @param saturation:           the positive saturation level
 * @param max_patterns:         the number of patterns
 * @param max_units:            the size of the network
 * @param remove_self_coupling  1 to suppress diagonal (else 0)
 */
void hn_saturated_weights_from_patterns(double **weights, spike_T **patterns,
					double saturation, int max_patterns,
					int max_units, int remove_self_coupling);


/**
 * Add the autocorrelation (outer product with itself) of pattern
 * to the matrix weights, the saturate in [-saturation, saturation];
 * the diagonal is suppressed iff remove_self_coupling is non-zero;
 * Incremental version of hn_hebb_weights_from_patterns().
 *
 * @param weights:              the weight matrix to be updated
 * @param pattern:              the pattern to be learnt
 * @param saturation:           the positive saturation level
 * @param max_units:            the size of the network
 * @param remove_self_coupling  1 to suppress diagonal (else 0)
 */
void hn_saturated_weights_increment_with_pattern(double **weights,
                                                 spike_T *pattern,
                                                 double saturation,
                                                 int max_units,
                                                 int remove_self_coupling);


#endif /* HN_NETWORK_H */
