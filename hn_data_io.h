/*****************************************************
 * HEADER FILE: hn_data_io.h                         *
 * MODULE: Data I/O, encoding/decoding               *
 *                                                   *
 * FUNCTION: Extract the weight matrix and initial   *
 *           patterns from data-files; encode and    *
 *           save [results] on a save-file           *
 *                                                   *
 * PROJECT NAME: Hopfield Network simulation         *
 *               (prototype)                         *
 *                                                   *
 * SUPERVISOR: Mark van Rossum                       *
 *                                                   *
 * AUTHOR:  Lorenzo Mella                            *
 * VERSION: 19/07/2016                               *
 *                                                   *
 *****************************************************/

#ifndef HN_DATA_ENC_H
#define HN_DATA_ENC_H

typedef enum io_error_code {
    IO_SUCCESS,
    IO_FAILURE
} io_error_code;

/*
 * The following functions require already allocated arrays as their first
 * argument and return a flag signalling correct execution or errors.
 */


/**
 * Reads the weights from a data-file; the user is responsible for providing
 * a pointer 'weights' to a max_units * max_units array.
 *
 * @param weights:      pointer to matrix that will hold the extracted weights
 * @param w_filename:   name of the datafile where the weights are stored
 * @param max_units:    the size of the network
 *
 * @return:             outcome (type io_error_code)
 */
io_error_code hn_read_weights(double **weights, char *w_filename,
                              size_t max_units);


/**
 * Reads a pattern from a data-file; the data-file is read sequence by sequence
 * until EOF is encountered; the user is responsible for providing a pointer
 * 'pattern' to a 'max_units' long array.
 *
 * @param pattern:      pointer to copy location of the extracted pattern
 * @param p_filename:   name of the datafile where the patterns are stored
 * @param max_units:    the size of the network
 *
 * @return:             outcome (type io_error_code)
 */
io_error_code hn_read_next_pattern(spike_T *pattern, char *p_filename,
                                   size_t max_units);


/**
 * Saves any list of doubles (e.g., avg overlap counts, timings, etc.)
 *
 * @param output:           the array to be saved
 * @param s_filename:       name of the savefile (overwrites if it exists)
 * @param output_length:    the length of the array
 *
 * @return:                 outcome (type io_error_code)
 */
io_error_code hn_save(double *output, char *s_filename, size_t output_length);


/**
 * Saves a weight matrix in a newly created file.
 *
 * @param weights:      the max_units * max_units weight matrix
 * @param w_filename:   name of the file to create and save to
 * @param max_units:    the size of the network
 *
 * @return:             outcome (type io_error_code)
 */
io_error_code hn_save_weights(double **weights, char *w_filename,
                              size_t max_units);


/**
 * Append a pattern to a binary file (newly created if it doesn't exist).
 *
 * @param pattern:      the pattern to be saved
 * @param p_filename:   the name of the file to save to
 * @param max_units:    the length of the pattern
 *
 * @return:             outcome (type io_error_code)
 */
io_error_code hn_save_next_pattern(spike_T *pattern, char *p_filename,
                                   size_t max_units);


/**
 * Creates a max_units-long pattern of spike_T values
 * with the specified coding level (probability of +1);
 * the pattern array is expected to be pre-allocated..
 *
 * @param pattern:      the pattern array to be filled
 * @param coding_level: the probability that a unit is extracted as +1
 * @param max_units:    the size of the network
 */
void hn_fill_rand_pattern(spike_T *pattern, double coding_level,
                          size_t max_units);


#endif
